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
    أرجع &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, فراغ *a)
{
    أكد(a != عدم);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    أرجع seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_قىستۇرۇش_in_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    إذا (!seq) {
        أرجع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    أرجع new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, فراغ *a)
{
    أكد(a != عدم);
    إذا (!seq) {
        أرجع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    أرجع new_seq;
}

ساكن Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    أرجع size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    أكد(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    إذا (!flattened_seq) {
        أرجع عدم;
    }

    صحيح flattened_seq_idx = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        لكل (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_size);

    أرجع flattened_seq;
}

فراغ *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أرجع asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    أرجع asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
ئىپادە_ty
_PyPegen_join_names_with_dot(Parser *p, ئىپادە_ty first_name, ئىپادە_ty second_name)
{
    أكد(first_name != عدم && second_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    إذا (!uni) {
        أرجع عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    إذا (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        أرجع عدم;
    }

    أرجع _PyAST_Name(uni, Load, EXTRA_ئىپادە(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
Py_Pegen_seq_سانى_dots(asdl_seq *seq)
{
    صحيح سان_of_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_ئىپادە = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_ئىپادە->type) {
            حالة ELLIPSIS:
                سان_of_dots += 3;
                اكسر;
            حالة DOT:
                سان_of_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    أرجع سان_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, صحيح lineno, صحيح col_offset, صحيح end_lineno,
                        صحيح end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    إذا (!str) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_alias(str, عدم, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_ئىپادە_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    أرجع new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_ئىپادە_Pair *
Py_Pegen_cmpop_ئىپادە_pair(Parser *p, cmpop_ty cmpop, ئىپادە_ty ئىپادە)
{
    أكد(ئىپادە != عدم);
    Cmpop_ئىپادە_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_ئىپادە_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->cmpop = cmpop;
    a->ئىپادە = ئىپادە;
    أرجع a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ئىپادە_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    أرجع new_seq;
}

asdl_ئىپادە_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_ئىپادە_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ئىپادە);
    }
    أرجع new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_ئىپادە_seq *
_set_seq_context(Parser *p, asdl_ئىپادە_seq *seq, ئىپادە_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إذا (len == 0) {
        أرجع عدم;
    }

    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty e = asdl_seq_GET(seq, i);
        ئىپادە_ty new_e = Py_Pegen_set_ئىپادە_context(p, e, ctx);
        إذا (!new_e) {
            أرجع عدم;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    أرجع new_seq;
}

ساكن ئىپادە_ty
_set_name_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    أرجع _PyAST_Name(e->v.Name.id, ctx, EXTRA_ئىپادە(e, e));
}

ساكن ئىپادە_ty
_set_tuple_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    asdl_ئىپادە_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_Tuple(seq, ctx, EXTRA_ئىپادە(e, e));
}

ساكن ئىپادە_ty
_set_list_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    asdl_ئىپادە_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_List(seq, ctx, EXTRA_ئىپادە(e, e));
}

ساكن ئىپادە_ty
_set_subscript_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    أرجع _PyAST_Subscript(e->v.Subscript.قىممەت, e->v.Subscript.slice,
                            ctx, EXTRA_ئىپادە(e, e));
}

ساكن ئىپادە_ty
_set_attribute_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    أرجع _PyAST_Attribute(e->v.Attribute.قىممەت, e->v.Attribute.attr,
                            ctx, EXTRA_ئىپادە(e, e));
}

ساكن ئىپادە_ty
_set_starred_context(Parser *p, ئىپادە_ty e, ئىپادە_context_ty ctx)
{
    ئىپادە_ty inner = Py_Pegen_set_ئىپادە_context(p, e->v.Starred.قىممەت, ctx);
    إذا (!inner) {
        أرجع عدم;
    }
    أرجع _PyAST_Starred(inner, ctx, EXTRA_ئىپادە(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
ئىپادە_ty
Py_Pegen_set_ئىپادە_context(Parser *p, ئىپادە_ty ئىپادە, ئىپادە_context_ty ctx)
{
    أكد(ئىپادە != عدم);

    ئىپادە_ty new = عدم;
    بدل (ئىپادە->kind) {
        حالة Name_kind:
            new = _set_name_context(p, ئىپادە, ctx);
            اكسر;
        حالة Tuple_kind:
            new = _set_tuple_context(p, ئىپادە, ctx);
            اكسر;
        حالة List_kind:
            new = _set_list_context(p, ئىپادە, ctx);
            اكسر;
        حالة Subscript_kind:
            new = _set_subscript_context(p, ئىپادە, ctx);
            اكسر;
        حالة Attribute_kind:
            new = _set_attribute_context(p, ئىپادە, ctx);
            اكسر;
        حالة Starred_kind:
            new = _set_starred_context(p, ئىپادە, ctx);
            اكسر;
        افتراضي:
            new = ئىپادە;
    }
    أرجع new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
ئىپادە_ty
_PyPegen_make_target(Parser *p, ئىپادە_ty ئىپادە, TARGETS_TYPE targets_type)
{
    أكد(ئىپادە != عدم);
    أكد(targets_type != FOR_TARGETS);
    إذا (_PyPegen_get_invalid_target(ئىپادە, targets_type) != عدم) {
        أرجع عدم;
    }
    أرجع Py_Pegen_set_ئىپادە_context(
        p, ئىپادە, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
ئاچقۇچ_قىممەت_Pair *
Py_Pegen_ئاچقۇچ_قىممەت_pair(Parser *p, ئىپادە_ty ئاچقۇچ, ئىپادە_ty قىممەت)
{
    ئاچقۇچ_قىممەت_Pair *a = _PyArena_Malloc(p->arena, حجم(ئاچقۇچ_قىممەت_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->ئاچقۇچ = ئاچقۇچ;
    a->قىممەت = قىممەت;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_ئىپادە_seq *
Py_Pegen_get_ئاچقۇچلار(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئاچقۇچ_قىممەت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ئاچقۇچ);
    }
    أرجع new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_ئىپادە_seq *
Py_Pegen_get_قىممەتلەر(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئاچقۇچ_قىممەت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->قىممەت);
    }
    أرجع new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
ئاچقۇچ_Pattern_Pair *
Py_Pegen_ئاچقۇچ_pattern_pair(Parser *p, ئىپادە_ty ئاچقۇچ, pattern_ty pattern)
{
    ئاچقۇچ_Pattern_Pair *a = _PyArena_Malloc(p->arena, حجم(ئاچقۇچ_Pattern_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->ئاچقۇچ = ئاچقۇچ;
    a->pattern = pattern;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_ئىپادە_seq *
Py_Pegen_get_pattern_ئاچقۇچلار(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئاچقۇچ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->ئاچقۇچ);
    }
    أرجع new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئاچقۇچ_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    أرجع new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, ئىپادە_ty قىممەت, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    إذا (!a) {
        أرجع عدم;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    إذا (!a->arg) {
        أرجع عدم;
    }
    a->قىممەت = قىممەت;
    أرجع a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    إذا (!a) {
        أرجع عدم;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    أرجع a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, حجم(StarEtc));
    إذا (!a) {
        أرجع عدم;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    أرجع a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    أرجع new_seq;
}

ساكن asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    أرجع seq;
}

ساكن asdl_ئىپادە_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_ئىپادە_seq *seq = Py_asdl_ئىپادە_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->قىممەت);
    }
    أرجع seq;
}

ساكن صحيح
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    إذا (slash_without_default != عدم) {
        *posonlyargs = slash_without_default;
    }
    وإلا إذا (slash_with_default != عدم) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        إذا (!slash_with_default_names) {
            أرجع -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    وإلا {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    أرجع *posonlyargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    إذا (names_with_default != عدم) {
        إذا (plain_names != عدم) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            إذا (!names_with_default_names) {
                أرجع -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        وإلا {
            *posargs = _get_names(p, names_with_default);
        }
    }
    وإلا {
        إذا (plain_names != عدم) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_names;
        }
        وإلا {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    أرجع *posargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_ئىپادە_seq **posdefaults) {
    إذا (slash_with_default != عدم && names_with_default != عدم) {
        asdl_ئىپادە_seq *slash_with_default_قىممەتلەر =
                _get_defaults(p, slash_with_default->names_with_defaults);
        إذا (!slash_with_default_قىممەتلەر) {
            أرجع -1;
        }
        asdl_ئىپادە_seq *names_with_default_قىممەتلەر = _get_defaults(p, names_with_default);
        إذا (!names_with_default_قىممەتلەر) {
            أرجع -1;
        }
        *posdefaults = (asdl_ئىپادە_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_قىممەتلەر,
                (asdl_seq*)names_with_default_قىممەتلەر);
    }
    وإلا إذا (slash_with_default == عدم && names_with_default != عدم) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    وإلا إذا (slash_with_default != عدم && names_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    وإلا {
        *posdefaults = Py_asdl_ئىپادە_seq_new(0, p->arena);
    }
    أرجع *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_ئىپادە_seq **kwdefaults) {
    إذا (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    وإلا {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    إذا (*kwonlyargs == عدم) {
        أرجع -1;
    }

    إذا (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    وإلا {
        *kwdefaults = Py_asdl_ئىپادە_seq_new(0, p->arena);
    }

    إذا (*kwdefaults == عدم) {
        أرجع -1;
    }

    أرجع 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    إذا (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        أرجع عدم;
    }

    asdl_arg_seq *posargs;
    إذا (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        أرجع عدم;
    }

    asdl_ئىپادە_seq *posdefaults;
    إذا (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty vararg = عدم;
    إذا (star_etc != عدم && star_etc->vararg != عدم) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_ئىپادە_seq *kwdefaults;
    إذا (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty kwarg = عدم;
    إذا (star_etc != عدم && star_etc->kwarg != عدم) {
        kwarg = star_etc->kwarg;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posonlyargs) {
        أرجع عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posargs) {
        أرجع عدم;
    }
    asdl_ئىپادە_seq *posdefaults = Py_asdl_ئىپادە_seq_new(0, p->arena);
    إذا (!posdefaults) {
        أرجع عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!kwonlyargs) {
        أرجع عدم;
    }
    asdl_ئىپادە_seq *kwdefaults = Py_asdl_ئىپادە_seq_new(0, p->arena);
    إذا (!kwdefaults) {
        أرجع عدم;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    إذا (!a) {
        أرجع عدم;
    }
    a->kind = kind;
    أرجع a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_ئىپادە_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    إذا (function_def->kind == AsyncFunctionDef_kind) {
        أرجع _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    أرجع _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_ئىپادە_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != عدم);
    أرجع _PyAST_ClassDef(
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
    إذا (!a) {
        أرجع عدم;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    أرجع a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ساكن صحيح
seq_سان_of_starred_exprs(asdl_seq *seq)
{
    صحيح n = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        إذا (!k->is_keyword) {
            n++;
        }
    }
    أرجع n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_ئىپادە_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = seq_سان_of_starred_exprs(kwargs);
    إذا (new_len == 0) {
        أرجع عدم;
    }
    asdl_ئىپادە_seq *new_seq = Py_asdl_ئىپادە_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_سان_of_starred_exprs(kwargs);
    إذا (new_len == 0) {
        أرجع عدم;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

ئىپادە_ty
_PyPegen_ensure_imaginary(Parser *p, ئىپادە_ty exp)
{
    إذا (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.قىممەت)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        أرجع عدم;
    }
    أرجع exp;
}

ئىپادە_ty
_PyPegen_ensure_real(Parser *p, ئىپادە_ty exp)
{
    إذا (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.قىممەت)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        أرجع عدم;
    }
    أرجع exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t رەقەم = p->type_ignore_comments.رەقەم_بۇيۇملار;
    إذا (رەقەم > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(رەقەم, p->arena);
        إذا (type_ignores == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < رەقەم; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.بۇيۇملار[i].comment);
            إذا (tag == عدم) {
                أرجع عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.بۇيۇملار[i].lineno,
                                                  tag, p->arena);
            إذا (ti == عدم) {
                أرجع عدم;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    أرجع _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), عدم);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    إذا (tc == عدم) {
        أرجع a;
    }
    ثابت حرف *bytes = PyBytes_AsString(tc->bytes);
    إذا (bytes == عدم) {
        أرجع عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    إذا (tco == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_arg(a->arg, a->annotation, tco,
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
    إذا (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        أرجع -1;
    }
    إذا (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        أرجع strcmp(tok_str, "!=");
    }
    أرجع 0;
}

صحيح
_PyPegen_check_legacy_stmt(Parser *p, ئىپادە_ty name) {
    إذا (name->kind != Name_kind) {
        أرجع 0;
    }
    ثابت حرف* candidates[2] = {"print", "exec"};
    لكل (صحيح i=0; i<2; i++) {
        إذا (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            أرجع 1;
        }
    }
    أرجع 0;
}

فراغ *
_PyPegen_raise_error_for_missing_comma(Parser *p, ئىپادە_ty a, ئىپادە_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    إذا (_PyPegen_check_legacy_stmt(p, a)) {
        أرجع عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    إذا (p->tokens[p->mark - 1]->level == 0) {
        أرجع عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    إذا (a->end_lineno > a->lineno) {
        أرجع RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    أرجع RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساكن ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, فراغ *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, حجم(ResultTokenWithMetadata));
    إذا (res == عدم) {
        أرجع عدم;
    }
    res->metadata = metadata;
    res->result = result;
    أرجع res;
}

ساكن حرف
formatted_string_prefix(ثابت Parser *p)
{
    صحيح nested = 0;
    لكل (صحيح i = p->mark - 1; i >= 0; i--) {
        صحيح type = p->tokens[i]->type;
        إذا (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        وإلا إذا (type == FSTRING_START || type == TSTRING_START) {
            إذا (nested == 0) {
                أرجع type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, ئىپادە_ty conv)
{
    إذا (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        أرجع RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    إذا (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        أرجع عدم;
    }

    أرجع result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_ئىپادە_seq *spec, صحيح lineno, صحيح col_offset,
                                صحيح end_lineno, صحيح end_col_offset, PyArena *arena)
{
    إذا (!spec) {
        أرجع عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_بۇيۇملار = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_سانى = 0;
    لكل (Py_ssize_t i = 0; i < n_بۇيۇملار; i++) {
        ئىپادە_ty item = asdl_seq_GET(spec, i);
        non_empty_سانى += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.قىممەت) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.قىممەت) == 0);
    }
    إذا (non_empty_سانى != n_بۇيۇملار) {
        asdl_ئىپادە_seq *resized_spec =
            Py_asdl_ئىپادە_seq_new(non_empty_سانى, p->arena);
        إذا (resized_spec == عدم) {
            أرجع عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_بۇيۇملار; i++) {
            ئىپادە_ty item = asdl_seq_GET(spec, i);
            إذا (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.قىممەت) &&
                PyUnicode_GET_LENGTH(item->v.Constant.قىممەت) == 0) {
                استمر;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        أكد(j == non_empty_سانى);
        spec = resized_spec;
    }
    ئىپادە_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    إذا (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } وإلا {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    إذا (!res) {
        أرجع عدم;
    }
    أرجع result_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_ئىپادە_name(ئىپادە_ty e)
{
    أكد(e != عدم);
    بدل (e->kind) {
        حالة Attribute_kind:
            أرجع "attribute";
        حالة Subscript_kind:
            أرجع "subscript";
        حالة Starred_kind:
            أرجع "starred";
        حالة Name_kind:
            أرجع "name";
        حالة List_kind:
            أرجع "list";
        حالة Tuple_kind:
            أرجع "tuple";
        حالة Lambda_kind:
            أرجع "lambda";
        حالة Call_kind:
            أرجع "function call";
        حالة BoolOp_kind:
        حالة BinOp_kind:
        حالة UnaryOp_kind:
            أرجع "expression";
        حالة GeneratorExp_kind:
            أرجع "generator expression";
        حالة Yield_kind:
        حالة YieldFrom_kind:
            أرجع "yield expression";
        حالة Await_kind:
            أرجع "await expression";
        حالة ListComp_kind:
            أرجع "list comprehension";
        حالة SetComp_kind:
            أرجع "set comprehension";
        حالة DictComp_kind:
            أرجع "dict comprehension";
        حالة Dict_kind:
            أرجع "dict literal";
        حالة Set_kind:
            أرجع "set display";
        حالة JoinedStr_kind:
        حالة Formatted_قىممەت_kind:
            أرجع "f-string expression";
        حالة TemplateStr_kind:
        حالة Interpolation_kind:
            أرجع "t-string expression";
        حالة Constant_kind: {
            PyObject *قىممەت = e->v.Constant.قىممەت;
            إذا (قىممەت == Py_None) {
                أرجع "None";
            }
            إذا (قىممەت == Py_False) {
                أرجع "False";
            }
            إذا (قىممەت == Py_True) {
                أرجع "True";
            }
            إذا (قىممەت == Py_Ellipsis) {
                أرجع "ellipsis";
            }
            أرجع "literal";
        }
        حالة Compare_kind:
            أرجع "comparison";
        حالة IfExp_kind:
            أرجع "conditional expression";
        حالة Named_ئىپادە_kind:
            أرجع "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            أرجع عدم;
    }
}

ئىپادە_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    إذا (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        أرجع comprehension->iter;
    }
    أرجع PyPegen_last_item(comprehension->ifs, ئىپادە_ty);
}

ئىپادە_ty _PyPegen_collect_call_seqs(Parser *p, asdl_ئىپادە_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح col_offset, صحيح end_lineno,
                     صحيح end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    إذا (b == عدم) {
        أرجع _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_ئىپادە_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    إذا (!starreds && PyErr_Occurred()) {
        أرجع عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    إذا (!keywords && PyErr_Occurred()) {
        أرجع عدم;
    }

    إذا (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_ئىپادە_seq *args = Py_asdl_ئىپادە_seq_new(total_len, arena);
    إذا (args == عدم) {
        أرجع عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    لكل (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    أرجع _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

ئىپادە_ty
_PyPegen_get_invalid_target(ئىپادە_ty e, TARGETS_TYPE targets_type)
{
    إذا (e == عدم) {
        أرجع عدم;
    }

    إذا (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        إذا (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            أرجع عدم;
        }
        أرجع e->kind == Attribute_kind || e->kind == Subscript_kind ? عدم : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) افعل { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            ئىپادە_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            ئىپادە_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            إذا (child != عدم) {\
                أرجع child;\
            }\
        }\
    } طالما (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->kind) {
        حالة List_kind:
            VISIT_CONTAINER(e, List);
            أرجع عدم;
        حالة Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            أرجع عدم;
        حالة Starred_kind:
            إذا (targets_type == DEL_TARGETS) {
                أرجع e;
            }
            أرجع _PyPegen_get_invalid_target(e->v.Starred.قىممەت, targets_type);
        حالة Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            إذا (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                إذا (cmpop == In) {
                    أرجع _PyPegen_get_invalid_target(e->v.Compare.سول, targets_type);
                }
                أرجع عدم;
            }
            أرجع e;
        حالة Name_kind:
        حالة Subscript_kind:
        حالة Attribute_kind:
            أرجع عدم;
        افتراضي:
            أرجع e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, ئىپادە_ty e) {
    صحيح kwarg_unpacking = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        إذا (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *ئۇچۇر = عدم;
    إذا (kwarg_unpacking) {
        ئۇچۇر = "positional argument follows keyword argument unpacking";
    } وإلا {
        ئۇچۇر = "positional argument follows keyword argument";
    }

    أرجع RAISE_SYNTAX_ERROR(ئۇچۇر);
}

فراغ *
_PyPegen_nonparen_genexp_in_call(Parser *p, ئىپادە_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    إذا (len <= 1) {
        أرجع عدم;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    أرجع RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (ئىپادە_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن ئىپادە_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, ئىپادە_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.قىممەت));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.قىممەت);
    إذا (bstr == عدم) {
        أرجع عدم;
    }

    size_t len;
    إذا (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } وإلا {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == عدم;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    إذا (str == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ساكن asdl_ئىپادە_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_ئىپادە_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_بۇيۇملار = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_بۇيۇملار = n_بۇيۇملار;
    لكل (Py_ssize_t i = 0; i < n_بۇيۇملار; i++) {
        ئىپادە_ty item = asdl_seq_GET(raw_expressions, i);
        إذا (item->kind == JoinedStr_kind) {
            total_بۇيۇملار += asdl_seq_LEN(item->v.JoinedStr.قىممەتلەر) - 1;
        }
    }

    ثابت حرف* quote_str = PyBytes_AsString(a->bytes);
    إذا (quote_str == عدم) {
        أرجع عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_ئىپادە_seq *seq = Py_asdl_ئىپادە_seq_new(total_بۇيۇملار, p->arena);
    إذا (seq == عدم) {
        أرجع عدم;
    }

    Py_ssize_t index = 0;
    لكل (Py_ssize_t i = 0; i < n_بۇيۇملار; i++) {
        ئىپادە_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        إذا (item->kind == JoinedStr_kind) {
            asdl_ئىپادە_seq *قىممەتلەر = item->v.JoinedStr.قىممەتلەر;
            إذا (asdl_seq_LEN(قىممەتلەر) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                أرجع عدم;
            }

            ئىپادە_ty first = asdl_seq_GET(قىممەتلەر, 0);
            أكد(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            ئىپادە_ty second = asdl_seq_GET(قىممەتلەر, 1);
            أكد((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_قىممەت_kind);
            asdl_seq_SET(seq, index++, second);

            استمر;
        }

        إذا (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            إذا (item == عدم) {
                أرجع عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            إذا (PyUnicode_CheckExact(item->v.Constant.قىممەت)
                && PyUnicode_GET_LENGTH(item->v.Constant.قىممەت) == 0) {
                استمر;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_ئىپادە_seq *resized_exprs;
    إذا (index != total_بۇيۇملار) {
        resized_exprs = Py_asdl_ئىپادە_seq_new(index, p->arena);
        إذا (resized_exprs == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وإلا {
        resized_exprs = seq;
    }
    أرجع resized_exprs;
}

ئىپادە_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_ئىپادە_seq *raw_expressions, Token *b) {

    asdl_ئىپادە_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

ئىپادە_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_ئىپادە_seq* raw_expressions, Token*b) {

    asdl_ئىپادە_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

ئىپادە_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        أرجع Py_Pegen_raise_incomplete_كىرگۈزۈش(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ئىپادە_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        أرجع Py_Pegen_raise_incomplete_كىرگۈزۈش(p);
    }
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

ئىپادە_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        أرجع Py_Pegen_raise_incomplete_كىرگۈزۈش(p);
    }
    ثابت حرف *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    إذا (s == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        أرجع عدم;
    }
    PyObject *kind = عدم;
    إذا (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        إذا (kind == عدم) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    إذا (conversion != عدم) {
        ئىپادە_ty conversion_ئىپادە = (ئىپادە_ty) conversion->result;
        أكد(conversion_ئىپادە->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_ئىپادە->v.Name.id, 0);
        أرجع Py_SAFE_DOWNCAST(first, Py_UCS4, صحيح);
    }
    وإلا إذا (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        أرجع (صحيح)'r';
    }
    أرجع -1;
}

ساكن PyObject *
strip_interpolation_debug_ئىپادە(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    طالما (len > 0) {
        صحيح has_newline = 0;
        طالما (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            إذا (!_PyUnicode_IsWhitespace(c)) {
                اكسر;
            }
            إذا (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        إذا (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            اكسر;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    إذا (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        أرجع Py_NewRef(exprstr);
    }

    أرجع PyUnicode_Substring(exprstr, 0, len - 1);
}

ئىپادە_ty _PyPegen_interpolation(Parser *p, ئىپادە_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_قۇر, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    إذا (conversion) {
        debug_end_قۇر = ((ئىپادە_ty) conversion->result)->lineno;
        debug_end_offset = ((ئىپادە_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_end_قۇر = ((ئىپادە_ty) format->result)->lineno;
        debug_end_offset = ((ئىپادە_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وإلا {
        debug_end_قۇر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_ئىپادە(exprstr)
        : Py_NewRef(exprstr);
    إذا (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        أرجع عدم;
    }

    ئىپادە_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (ئىپادە_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    إذا (!interpolation || !debug) {
        أرجع interpolation;
    }

    ئىپادە_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_قۇر,
                                            debug_end_offset - 1, p->arena);
    إذا (!debug_text) {
        أرجع عدم;
    }

    asdl_ئىپادە_seq *قىممەتلەر = Py_asdl_ئىپادە_seq_new(2, arena);
    إذا (!قىممەتلەر) {
        أرجع عدم;
    }
    asdl_seq_SET(قىممەتلەر, 0, debug_text);
    asdl_seq_SET(قىممەتلەر, 1, interpolation);
    أرجع _PyAST_JoinedStr(قىممەتلەر, lineno, col_offset, debug_end_قۇر, debug_end_offset, p->arena);
}

ئىپادە_ty Py_Pegen_formatted_قىممەت(Parser *p, ئىپادە_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    ئىپادە_ty formatted_قىممەت = Py_AST_Formatted_قىممەت(
        expression, conversion_val, format ? (ئىپادە_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    إذا (!formatted_قىممەت || !debug) {
        أرجع formatted_قىممەت;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_قۇر, debug_end_offset;
    PyObject *debug_metadata;

    إذا (conversion) {
        debug_end_قۇر = ((ئىپادە_ty) conversion->result)->lineno;
        debug_end_offset = ((ئىپادە_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_end_قۇر = ((ئىپادە_ty) format->result)->lineno;
        debug_end_offset = ((ئىپادە_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    وإلا {
        debug_end_قۇر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    ئىپادە_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_قۇر,
                                            debug_end_offset - 1, p->arena);
    إذا (!debug_text) {
        أرجع عدم;
    }

    asdl_ئىپادە_seq *قىممەتلەر = Py_asdl_ئىپادە_seq_new(2, arena);
    إذا (!قىممەتلەر) {
        أرجع عدم;
    }
    asdl_seq_SET(قىممەتلەر, 0, debug_text);
    asdl_seq_SET(قىممەتلەر, 1, formatted_قىممەت);
    أرجع _PyAST_JoinedStr(قىممەتلەر, lineno, col_offset, debug_end_قۇر, debug_end_offset, p->arena);
}

ساكن ئىپادە_ty
_build_concatenated_bytes(Parser *p, asdl_ئىپادە_seq *strings, صحيح lineno,
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
        ئىپادە_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.قىممەت;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        إذا (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            أرجع عدم;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    إذا (writer == عدم) {
        أرجع عدم;
    }
    حرف *out = PyBytesWriter_GetData(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.قىممەت;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        إذا (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ساكن ئىپادە_ty
_build_concatenated_unicode(Parser *p, asdl_ئىپادە_seq *strings, صحيح lineno,
                        صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 1);

    ئىپادە_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    إذا (writer == عدم) {
        أرجع عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty current_elem = asdl_seq_GET(strings, i);
        أكد(current_elem->kind == Constant_kind);

        إذا (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.قىممەت)) {
            PyUnicodeWriter_Discard(writer);
            أرجع عدم;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    إذا (final == عدم) {
        أرجع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ساكن asdl_ئىپادە_seq *
_build_concatenated_str(Parser *p, asdl_ئىپادە_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.قىممەتلەر);
                اكسر;
            حالة TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.قىممەتلەر);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_ئىپادە_seq* flattened = Py_asdl_ئىپادە_seq_new(n_flattened_elements, p->arena);
    إذا (flattened == عدم) {
        أرجع عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        ئىپادە_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.قىممەتلەر); j++) {
                    ئىپادە_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.قىممەتلەر, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة TemplateStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.قىممەتلەر); j++) {
                    ئىپادە_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.قىممەتلەر, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
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
        ئىپادە_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        إذا (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.قىممەت) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.قىممەت) == 0)
            استمر;

        إذا (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_ئىپادە_seq* قىممەتلەر = Py_asdl_ئىپادە_seq_new(n_elements, p->arena);
    إذا (قىممەتلەر == عدم) {
        أرجع عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        ئىپادە_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        إذا (elem->kind == Constant_kind) {
            إذا (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                ئىپادە_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                إذا (writer == عدم) {
                    أرجع عدم;
                }
                ئىپادە_ty last_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    ئىپادە_ty current_elem = asdl_seq_GET(flattened, j);
                    إذا (current_elem->kind == Constant_kind) {
                        إذا (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.قىممەت)) {
                            PyUnicodeWriter_Discard(writer);
                            أرجع عدم;
                        }
                        last_elem = current_elem;
                    } وإلا {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                إذا (concat_str == عدم) {
                    أرجع عدم;
                }
                إذا (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    أرجع عدم;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                إذا (elem == عدم) {
                    أرجع عدم;
                }
            }

            /* Drop all empty contanst strings */
            إذا (PyUnicode_CheckExact(elem->v.Constant.قىممەت) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.قىممەت) == 0) {
                استمر;
            }
        }

        asdl_seq_SET(قىممەتلەر, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    أرجع قىممەتلەر;
}

ساكن ئىپادە_ty
_build_concatenated_joined_str(Parser *p, asdl_ئىپادە_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_ئىپادە_seq *قىممەتلەر = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    إذا (!قىممەتلەر) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(قىممەتلەر, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ئىپادە_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_ئىپادە_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_ئىپادە_seq *قىممەتلەر = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    إذا (!قىممەتلەر) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(قىممەتلەر, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

ئىپادە_ty
_PyPegen_concatenate_strings(Parser *p, asdl_ئىپادە_seq *strings,
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
        ئىپادە_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة Constant_kind:
                إذا (PyBytes_CheckExact(elem->v.Constant.قىممەت)) {
                    bytes_found = 1;
                } وإلا {
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
    إذا ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        أرجع عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    إذا (!f_string_found) {
        إذا (len == 1) {
            أرجع asdl_seq_GET(strings, 0);
        }
        وإلا إذا (bytes_found) {
            أرجع _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        وإلا {
            أرجع _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    أرجع _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ساكن صحيح
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, ئىپادە_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    إذا (p->call_invalid_rules) {
        أرجع 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    إذا (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        أرجع 0;
    }

    صحيح سانى = Py_Pegen_seq_سانى_dots(dots);
    حرف *buf = PyMem_RawMalloc(سانى + 1);
    إذا (buf == عدم) {
        PyErr_NoMemory();
        أرجع -1;
    }
    memset(buf, '.', سانى);
    buf[سانى] = '\0';

    PyObject *ئۇچۇر = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    إذا (ئۇچۇر == عدم) {
        أرجع -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحيح res = _PyErr_EmitSyntaxWarning(ئۇچۇر,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(ئۇچۇر);
    أرجع res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, ئىپادە_ty module_name,
                             asdl_alias_seq *names, ئىپادە_ty lazy_token, صحيح lineno,
                             صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = Py_Pegen_seq_سانى_dots(dots);
    إذا (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        إذا (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            إذا (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    وإلا إذا (
        level > 0
        && lazy_token == عدم
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        إذا (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    إذا (!p->call_invalid_rules) {
        أرجع stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    إذا (len == 0) {
        أرجع stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    إذا (p->last_stmt_location.lineno > last_stmt->lineno) {
        أرجع stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    أرجع stmts;
}
