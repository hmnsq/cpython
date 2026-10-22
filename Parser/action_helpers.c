#அந்தர்பாவ <Python.h>
#அந்தர்பாவ "pycore_pystate.h"         // _PyInterpreterState_GET()
#அந்தர்பாவ "pycore_runtime.h"         // _PyRuntime
#அந்தர்பாவ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#அந்தர்பாவ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#அந்தர்பாவ "pegen.h"
#அந்தர்பாவ "string_parser.h"          // _PyPegen_decode_string()
#அந்தர்பாவ "lexer/state.h"            // tok_state


வெறுமை *
_PyPegen_dummy_name(Parser *p, ...)
{
    திரும்பு &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, வெறுமை *a)
{
    ப்ரதிஜ்ஞா(a != வெற்று);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    எனில் (!seq) {
        திரும்பு வெற்று;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    திரும்பு seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_செருகு_in_front(Parser *p, வெறுமை *a, asdl_seq *seq)
{
    ப்ரதிஜ்ஞா(a != வெற்று);
    எனில் (!seq) {
        திரும்பு _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    க்கு (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    திரும்பு new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, வெறுமை *a)
{
    ப்ரதிஜ்ஞா(a != வெற்று);
    எனில் (!seq) {
        திரும்பு _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }

    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    திரும்பு new_seq;
}

நிலையான Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    திரும்பு size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    ப்ரதிஜ்ஞா(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    எனில் (!flattened_seq) {
        திரும்பு வெற்று;
    }

    முழு flattened_seq_idx = 0;
    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        க்கு (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ப்ரதிஜ்ஞா(flattened_seq_idx == flattened_seq_size);

    திரும்பு flattened_seq;
}

வெறுமை *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    திரும்பு asdl_seq_GET_UNTYPED(seq, len - 1);
}

வெறுமை *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    திரும்பு asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
கோவை_ty
_PyPegen_join_names_with_dot(Parser *p, கோவை_ty first_name, கோவை_ty second_name)
{
    ப்ரதிஜ்ஞா(first_name != வெற்று && second_name != வெற்று);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    எனில் (!uni) {
        திரும்பு வெற்று;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    எனில் (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        திரும்பு வெற்று;
    }

    திரும்பு _PyAST_Name(uni, Load, EXTRA_கோவை(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
முழு
Py_Pegen_seq_மொத்தம்_dots(asdl_seq *seq)
{
    முழு எண்_of_dots = 0;
    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_கோவை = asdl_seq_GET_UNTYPED(seq, i);
        தேர்வு (current_கோவை->type) {
            நிலை ELLIPSIS:
                எண்_of_dots += 3;
                நிறுத்து;
            நிலை DOT:
                எண்_of_dots += 1;
                நிறுத்து;
            இயல்பு:
                Py_UNREACHABLE();
        }
    }

    திரும்பு எண்_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, முழு lineno, முழு col_offset, முழு end_lineno,
                        முழு end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    எனில் (!str) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_alias(str, வெற்று, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_கோவை_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ப்ரதிஜ்ஞா(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    திரும்பு new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_கோவை_Pair *
Py_Pegen_cmpop_கோவை_pair(Parser *p, cmpop_ty cmpop, கோவை_ty கோவை)
{
    ப்ரதிஜ்ஞா(கோவை != வெற்று);
    Cmpop_கோவை_Pair *a = _PyArena_Malloc(p->arena, அளவு(Cmpop_கோவை_Pair));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->cmpop = cmpop;
    a->கோவை = கோவை;
    திரும்பு a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ப்ரதிஜ்ஞா(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_கோவை_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    திரும்பு new_seq;
}

asdl_கோவை_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ப்ரதிஜ்ஞா(len > 0);

    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_கோவை_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->கோவை);
    }
    திரும்பு new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
நிலையான asdl_கோவை_seq *
_set_seq_context(Parser *p, asdl_கோவை_seq *seq, கோவை_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    எனில் (len == 0) {
        திரும்பு வெற்று;
    }

    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty e = asdl_seq_GET(seq, i);
        கோவை_ty new_e = Py_Pegen_set_கோவை_context(p, e, ctx);
        எனில் (!new_e) {
            திரும்பு வெற்று;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    திரும்பு new_seq;
}

நிலையான கோவை_ty
_set_name_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    திரும்பு _PyAST_Name(e->v.Name.id, ctx, EXTRA_கோவை(e, e));
}

நிலையான கோவை_ty
_set_tuple_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    asdl_கோவை_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    எனில் (!seq && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Tuple(seq, ctx, EXTRA_கோவை(e, e));
}

நிலையான கோவை_ty
_set_list_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    asdl_கோவை_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    எனில் (!seq && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_List(seq, ctx, EXTRA_கோவை(e, e));
}

நிலையான கோவை_ty
_set_subscript_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    திரும்பு _PyAST_Subscript(e->v.Subscript.மதிப்பு, e->v.Subscript.slice,
                            ctx, EXTRA_கோவை(e, e));
}

நிலையான கோவை_ty
_set_attribute_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    திரும்பு _PyAST_Attribute(e->v.Attribute.மதிப்பு, e->v.Attribute.attr,
                            ctx, EXTRA_கோவை(e, e));
}

நிலையான கோவை_ty
_set_starred_context(Parser *p, கோவை_ty e, கோவை_context_ty ctx)
{
    கோவை_ty inner = Py_Pegen_set_கோவை_context(p, e->v.Starred.மதிப்பு, ctx);
    எனில் (!inner) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Starred(inner, ctx, EXTRA_கோவை(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
கோவை_ty
Py_Pegen_set_கோவை_context(Parser *p, கோவை_ty கோவை, கோவை_context_ty ctx)
{
    ப்ரதிஜ்ஞா(கோவை != வெற்று);

    கோவை_ty new = வெற்று;
    தேர்வு (கோவை->kind) {
        நிலை Name_kind:
            new = _set_name_context(p, கோவை, ctx);
            நிறுத்து;
        நிலை Tuple_kind:
            new = _set_tuple_context(p, கோவை, ctx);
            நிறுத்து;
        நிலை List_kind:
            new = _set_list_context(p, கோவை, ctx);
            நிறுத்து;
        நிலை Subscript_kind:
            new = _set_subscript_context(p, கோவை, ctx);
            நிறுத்து;
        நிலை Attribute_kind:
            new = _set_attribute_context(p, கோவை, ctx);
            நிறுத்து;
        நிலை Starred_kind:
            new = _set_starred_context(p, கோவை, ctx);
            நிறுத்து;
        இயல்பு:
            new = கோவை;
    }
    திரும்பு new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
கோவை_ty
_PyPegen_make_target(Parser *p, கோவை_ty கோவை, TARGETS_TYPE targets_type)
{
    ப்ரதிஜ்ஞா(கோவை != வெற்று);
    ப்ரதிஜ்ஞா(targets_type != FOR_TARGETS);
    எனில் (_PyPegen_get_invalid_target(கோவை, targets_type) != வெற்று) {
        திரும்பு வெற்று;
    }
    திரும்பு Py_Pegen_set_கோவை_context(
        p, கோவை, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
சாவி_மதிப்பு_Pair *
Py_Pegen_சாவி_மதிப்பு_pair(Parser *p, கோவை_ty சாவி, கோவை_ty மதிப்பு)
{
    சாவி_மதிப்பு_Pair *a = _PyArena_Malloc(p->arena, அளவு(சாவி_மதிப்பு_Pair));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->சாவி = சாவி;
    a->மதிப்பு = மதிப்பு;
    திரும்பு a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_கோவை_seq *
Py_Pegen_get_சாவிகள்(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        சாவி_மதிப்பு_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->சாவி);
    }
    திரும்பு new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_கோவை_seq *
Py_Pegen_get_மதிப்புகள்(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        சாவி_மதிப்பு_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->மதிப்பு);
    }
    திரும்பு new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
சாவி_Pattern_Pair *
Py_Pegen_சாவி_pattern_pair(Parser *p, கோவை_ty சாவி, pattern_ty pattern)
{
    சாவி_Pattern_Pair *a = _PyArena_Malloc(p->arena, அளவு(சாவி_Pattern_Pair));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->சாவி = சாவி;
    a->pattern = pattern;
    திரும்பு a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_கோவை_seq *
Py_Pegen_get_pattern_சாவிகள்(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        சாவி_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->சாவி);
    }
    திரும்பு new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        சாவி_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    திரும்பு new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, கோவை_ty மதிப்பு, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, அளவு(NameDefaultPair));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    எனில் (!a->arg) {
        திரும்பு வெற்று;
    }
    a->மதிப்பு = மதிப்பு;
    திரும்பு a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, அளவு(SlashWithDefault));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    திரும்பு a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, அளவு(StarEtc));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    திரும்பு a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }

    முழு k = 0;
    க்கு (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    க்கு (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    திரும்பு new_seq;
}

நிலையான asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    எனில் (!seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    திரும்பு seq;
}

நிலையான asdl_கோவை_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_கோவை_seq *seq = Py_asdl_கோவை_seq_new(len, p->arena);
    எனில் (!seq) {
        திரும்பு வெற்று;
    }
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->மதிப்பு);
    }
    திரும்பு seq;
}

நிலையான முழு
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    எனில் (slash_without_default != வெற்று) {
        *posonlyargs = slash_without_default;
    }
    இல்லையெனில் எனில் (slash_with_default != வெற்று) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        எனில் (!slash_with_default_names) {
            திரும்பு -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    இல்லையெனில் {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    திரும்பு *posonlyargs == வெற்று ? -1 : 0;
}

நிலையான முழு
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    எனில் (names_with_default != வெற்று) {
        எனில் (plain_names != வெற்று) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            எனில் (!names_with_default_names) {
                திரும்பு -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        இல்லையெனில் {
            *posargs = _get_names(p, names_with_default);
        }
    }
    இல்லையெனில் {
        எனில் (plain_names != வெற்று) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ப்ரதிஜ்ஞா(0);
            *posargs = plain_names;
        }
        இல்லையெனில் {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    திரும்பு *posargs == வெற்று ? -1 : 0;
}

நிலையான முழு
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_கோவை_seq **posdefaults) {
    எனில் (slash_with_default != வெற்று && names_with_default != வெற்று) {
        asdl_கோவை_seq *slash_with_default_மதிப்புகள் =
                _get_defaults(p, slash_with_default->names_with_defaults);
        எனில் (!slash_with_default_மதிப்புகள்) {
            திரும்பு -1;
        }
        asdl_கோவை_seq *names_with_default_மதிப்புகள் = _get_defaults(p, names_with_default);
        எனில் (!names_with_default_மதிப்புகள்) {
            திரும்பு -1;
        }
        *posdefaults = (asdl_கோவை_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_மதிப்புகள்,
                (asdl_seq*)names_with_default_மதிப்புகள்);
    }
    இல்லையெனில் எனில் (slash_with_default == வெற்று && names_with_default != வெற்று) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    இல்லையெனில் எனில் (slash_with_default != வெற்று && names_with_default == வெற்று) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    இல்லையெனில் {
        *posdefaults = Py_asdl_கோவை_seq_new(0, p->arena);
    }
    திரும்பு *posdefaults == வெற்று ? -1 : 0;
}

நிலையான முழு
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_கோவை_seq **kwdefaults) {
    எனில் (star_etc != வெற்று && star_etc->kwonlyargs != வெற்று) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    இல்லையெனில் {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    எனில் (*kwonlyargs == வெற்று) {
        திரும்பு -1;
    }

    எனில் (star_etc != வெற்று && star_etc->kwonlyargs != வெற்று) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    இல்லையெனில் {
        *kwdefaults = Py_asdl_கோவை_seq_new(0, p->arena);
    }

    எனில் (*kwdefaults == வெற்று) {
        திரும்பு -1;
    }

    திரும்பு 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    எனில் (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        திரும்பு வெற்று;
    }

    asdl_arg_seq *posargs;
    எனில் (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        திரும்பு வெற்று;
    }

    asdl_கோவை_seq *posdefaults;
    எனில் (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        திரும்பு வெற்று;
    }

    arg_ty vararg = வெற்று;
    எனில் (star_etc != வெற்று && star_etc->vararg != வெற்று) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_கோவை_seq *kwdefaults;
    எனில் (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        திரும்பு வெற்று;
    }

    arg_ty kwarg = வெற்று;
    எனில் (star_etc != வெற்று && star_etc->kwarg != வெற்று) {
        kwarg = star_etc->kwarg;
    }

    திரும்பு _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    எனில் (!posonlyargs) {
        திரும்பு வெற்று;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    எனில் (!posargs) {
        திரும்பு வெற்று;
    }
    asdl_கோவை_seq *posdefaults = Py_asdl_கோவை_seq_new(0, p->arena);
    எனில் (!posdefaults) {
        திரும்பு வெற்று;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    எனில் (!kwonlyargs) {
        திரும்பு வெற்று;
    }
    asdl_கோவை_seq *kwdefaults = Py_asdl_கோவை_seq_new(0, p->arena);
    எனில் (!kwdefaults) {
        திரும்பு வெற்று;
    }

    திரும்பு _PyAST_arguments(posonlyargs, posargs, வெற்று, kwonlyargs,
                            kwdefaults, வெற்று, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, அளவு(AugOperator));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->kind = kind;
    திரும்பு a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_கோவை_seq *decorators, stmt_ty function_def)
{
    ப்ரதிஜ்ஞா(function_def != வெற்று);
    எனில் (function_def->kind == AsyncFunctionDef_kind) {
        திரும்பு _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    திரும்பு _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_கோவை_seq *decorators, stmt_ty class_def)
{
    ப்ரதிஜ்ஞா(class_def != வெற்று);
    திரும்பு _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, வெறுமை *element, முழு is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, அளவு(KeywordOrStarred));
    எனில் (!a) {
        திரும்பு வெற்று;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    திரும்பு a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
நிலையான முழு
seq_எண்_of_starred_exprs(asdl_seq *seq)
{
    முழு n = 0;
    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        எனில் (!k->is_keyword) {
            n++;
        }
    }
    திரும்பு n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_கோவை_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    முழு new_len = seq_எண்_of_starred_exprs(kwargs);
    எனில் (new_len == 0) {
        திரும்பு வெற்று;
    }
    asdl_கோவை_seq *new_seq = Py_asdl_கோவை_seq_new(new_len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }

    முழு idx = 0;
    க்கு (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        எனில் (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    திரும்பு new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_எண்_of_starred_exprs(kwargs);
    எனில் (new_len == 0) {
        திரும்பு வெற்று;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    எனில் (!new_seq) {
        திரும்பு வெற்று;
    }

    முழு idx = 0;
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        எனில் (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    திரும்பு new_seq;
}

கோவை_ty
_PyPegen_ensure_imaginary(Parser *p, கோவை_ty exp)
{
    எனில் (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.மதிப்பு)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        திரும்பு வெற்று;
    }
    திரும்பு exp;
}

கோவை_ty
_PyPegen_ensure_real(Parser *p, கோவை_ty exp)
{
    எனில் (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.மதிப்பு)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        திரும்பு வெற்று;
    }
    திரும்பு exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = வெற்று;
    Py_ssize_t இலக்கம் = p->type_ignore_comments.இலக்கம்_பொருட்கள்;
    எனில் (இலக்கம் > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(இலக்கம், p->arena);
        எனில் (type_ignores == வெற்று) {
            திரும்பு வெற்று;
        }
        க்கு (Py_ssize_t i = 0; i < இலக்கம்; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.பொருட்கள்[i].comment);
            எனில் (tag == வெற்று) {
                திரும்பு வெற்று;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.பொருட்கள்[i].lineno,
                                                  tag, p->arena);
            எனில் (ti == வெற்று) {
                திரும்பு வெற்று;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    திரும்பு _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, மாறிலி எழுத்து *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), வெற்று);
    எனில் (res == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        திரும்பு வெற்று;
    }
    திரும்பு res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    எனில் (tc == வெற்று) {
        திரும்பு a;
    }
    மாறிலி எழுத்து *bytes = PyBytes_AsString(tc->bytes);
    எனில் (bytes == வெற்று) {
        திரும்பு வெற்று;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    எனில் (tco == வெற்று) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
முழு
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ப்ரதிஜ்ஞா(t->bytes != வெற்று);
    ப்ரதிஜ்ஞா(t->type == NOTEQUAL);

    மாறிலி எழுத்து* tok_str = PyBytes_AS_STRING(t->bytes);
    எனில் (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        திரும்பு -1;
    }
    எனில் (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        திரும்பு strcmp(tok_str, "!=");
    }
    திரும்பு 0;
}

முழு
_PyPegen_check_legacy_stmt(Parser *p, கோவை_ty name) {
    எனில் (name->kind != Name_kind) {
        திரும்பு 0;
    }
    மாறிலி எழுத்து* candidates[2] = {"print", "exec"};
    க்கு (முழு i=0; i<2; i++) {
        எனில் (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            திரும்பு 1;
        }
    }
    திரும்பு 0;
}

வெறுமை *
_PyPegen_raise_error_for_missing_comma(Parser *p, கோவை_ty a, கோவை_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    எனில் (_PyPegen_check_legacy_stmt(p, a)) {
        திரும்பு வெற்று;
    }
    // Only raise inside parentheses/brackets (level > 0)
    எனில் (p->tokens[p->mark - 1]->level == 0) {
        திரும்பு வெற்று;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    எனில் (a->end_lineno > a->lineno) {
        திரும்பு RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    திரும்பு RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

நிலையான ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, வெறுமை *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, அளவு(ResultTokenWithMetadata));
    எனில் (res == வெற்று) {
        திரும்பு வெற்று;
    }
    res->metadata = metadata;
    res->result = result;
    திரும்பு res;
}

நிலையான எழுத்து
formatted_string_prefix(மாறிலி Parser *p)
{
    முழு nested = 0;
    க்கு (முழு i = p->mark - 1; i >= 0; i--) {
        முழு type = p->tokens[i]->type;
        எனில் (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        இல்லையெனில் எனில் (type == FSTRING_START || type == TSTRING_START) {
            எனில் (nested == 0) {
                திரும்பு type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, கோவை_ty conv)
{
    எனில் (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        திரும்பு RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    எனில் (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        திரும்பு வெற்று;
    }

    திரும்பு result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_கோவை_seq *spec, முழு lineno, முழு col_offset,
                                முழு end_lineno, முழு end_col_offset, PyArena *arena)
{
    எனில் (!spec) {
        திரும்பு வெற்று;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_பொருட்கள் = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_மொத்தம் = 0;
    க்கு (Py_ssize_t i = 0; i < n_பொருட்கள்; i++) {
        கோவை_ty item = asdl_seq_GET(spec, i);
        non_empty_மொத்தம் += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.மதிப்பு) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.மதிப்பு) == 0);
    }
    எனில் (non_empty_மொத்தம் != n_பொருட்கள்) {
        asdl_கோவை_seq *resized_spec =
            Py_asdl_கோவை_seq_new(non_empty_மொத்தம், p->arena);
        எனில் (resized_spec == வெற்று) {
            திரும்பு வெற்று;
        }
        Py_ssize_t j = 0;
        க்கு (Py_ssize_t i = 0; i < n_பொருட்கள்; i++) {
            கோவை_ty item = asdl_seq_GET(spec, i);
            எனில் (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.மதிப்பு) &&
                PyUnicode_GET_LENGTH(item->v.Constant.மதிப்பு) == 0) {
                தொடர்;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        ப்ரதிஜ்ஞா(j == non_empty_மொத்தம்);
        spec = resized_spec;
    }
    கோவை_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    எனில் (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } இல்லையெனில் {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    எனில் (!res) {
        திரும்பு வெற்று;
    }
    திரும்பு result_token_with_metadata(p, res, colon->metadata);
}

மாறிலி எழுத்து *
Py_Pegen_get_கோவை_name(கோவை_ty e)
{
    ப்ரதிஜ்ஞா(e != வெற்று);
    தேர்வு (e->kind) {
        நிலை Attribute_kind:
            திரும்பு "attribute";
        நிலை Subscript_kind:
            திரும்பு "subscript";
        நிலை Starred_kind:
            திரும்பு "starred";
        நிலை Name_kind:
            திரும்பு "name";
        நிலை List_kind:
            திரும்பு "list";
        நிலை Tuple_kind:
            திரும்பு "tuple";
        நிலை Lambda_kind:
            திரும்பு "lambda";
        நிலை Call_kind:
            திரும்பு "function call";
        நிலை BoolOp_kind:
        நிலை BinOp_kind:
        நிலை UnaryOp_kind:
            திரும்பு "expression";
        நிலை GeneratorExp_kind:
            திரும்பு "generator expression";
        நிலை Yield_kind:
        நிலை YieldFrom_kind:
            திரும்பு "yield expression";
        நிலை Await_kind:
            திரும்பு "await expression";
        நிலை ListComp_kind:
            திரும்பு "list comprehension";
        நிலை SetComp_kind:
            திரும்பு "set comprehension";
        நிலை DictComp_kind:
            திரும்பு "dict comprehension";
        நிலை Dict_kind:
            திரும்பு "dict literal";
        நிலை Set_kind:
            திரும்பு "set display";
        நிலை JoinedStr_kind:
        நிலை Formatted_மதிப்பு_kind:
            திரும்பு "f-string expression";
        நிலை TemplateStr_kind:
        நிலை Interpolation_kind:
            திரும்பு "t-string expression";
        நிலை Constant_kind: {
            PyObject *மதிப்பு = e->v.Constant.மதிப்பு;
            எனில் (மதிப்பு == Py_None) {
                திரும்பு "None";
            }
            எனில் (மதிப்பு == Py_False) {
                திரும்பு "False";
            }
            எனில் (மதிப்பு == Py_True) {
                திரும்பு "True";
            }
            எனில் (மதிப்பு == Py_Ellipsis) {
                திரும்பு "ellipsis";
            }
            திரும்பு "literal";
        }
        நிலை Compare_kind:
            திரும்பு "comparison";
        நிலை IfExp_kind:
            திரும்பு "conditional expression";
        நிலை Named_கோவை_kind:
            திரும்பு "named expression";
        இயல்பு:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            திரும்பு வெற்று;
    }
}

கோவை_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    எனில் (comprehension->ifs == வெற்று || asdl_seq_LEN(comprehension->ifs) == 0) {
        திரும்பு comprehension->iter;
    }
    திரும்பு PyPegen_last_item(comprehension->ifs, கோவை_ty);
}

கோவை_ty _PyPegen_collect_call_seqs(Parser *p, asdl_கோவை_seq *a, asdl_seq *b,
                     முழு lineno, முழு col_offset, முழு end_lineno,
                     முழு end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    எனில் (b == வெற்று) {
        திரும்பு _PyAST_Call(_PyPegen_dummy_name(p), a, வெற்று, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_கோவை_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    எனில் (!starreds && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    எனில் (!keywords && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }

    எனில் (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_கோவை_seq *args = Py_asdl_கோவை_seq_new(total_len, arena);
    எனில் (args == வெற்று) {
        திரும்பு வெற்று;
    }

    Py_ssize_t i = 0;
    க்கு (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    க்கு (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    திரும்பு _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

கோவை_ty
_PyPegen_get_invalid_target(கோவை_ty e, TARGETS_TYPE targets_type)
{
    எனில் (e == வெற்று) {
        திரும்பு வெற்று;
    }

    எனில் (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        எனில் (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            திரும்பு வெற்று;
        }
        திரும்பு e->kind == Attribute_kind || e->kind == Subscript_kind ? வெற்று : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) செய் { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        க்கு (Py_ssize_t i = 0; i < len; i++) {\
            கோவை_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            கோவை_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            எனில் (child != வெற்று) {\
                திரும்பு child;\
            }\
        }\
    } போது (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    தேர்வு (e->kind) {
        நிலை List_kind:
            VISIT_CONTAINER(e, List);
            திரும்பு வெற்று;
        நிலை Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            திரும்பு வெற்று;
        நிலை Starred_kind:
            எனில் (targets_type == DEL_TARGETS) {
                திரும்பு e;
            }
            திரும்பு _PyPegen_get_invalid_target(e->v.Starred.மதிப்பு, targets_type);
        நிலை Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            எனில் (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                எனில் (cmpop == In) {
                    திரும்பு _PyPegen_get_invalid_target(e->v.Compare.இடது, targets_type);
                }
                திரும்பு வெற்று;
            }
            திரும்பு e;
        நிலை Name_kind:
        நிலை Subscript_kind:
        நிலை Attribute_kind:
            திரும்பு வெற்று;
        இயல்பு:
            திரும்பு e;
    }
}

வெறுமை *_PyPegen_arguments_parsing_error(Parser *p, கோவை_ty e) {
    முழு kwarg_unpacking = 0;
    க்கு (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        எனில் (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    மாறிலி எழுத்து *செய்தி = வெற்று;
    எனில் (kwarg_unpacking) {
        செய்தி = "positional argument follows keyword argument unpacking";
    } இல்லையெனில் {
        செய்தி = "positional argument follows keyword argument";
    }

    திரும்பு RAISE_SYNTAX_ERROR(செய்தி);
}

வெறுமை *
_PyPegen_nonparen_genexp_in_call(Parser *p, கோவை_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    எனில் (len <= 1) {
        திரும்பு வெற்று;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    திரும்பு RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (கோவை_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

நிலையான கோவை_ty
_PyPegen_decode_fstring_part(Parser* p, முழு is_raw, கோவை_ty constant, Token* token) {
    ப்ரதிஜ்ஞா(PyUnicode_CheckExact(constant->v.Constant.மதிப்பு));

    மாறிலி எழுத்து* bstr = PyUnicode_AsUTF8(constant->v.Constant.மதிப்பு);
    எனில் (bstr == வெற்று) {
        திரும்பு வெற்று;
    }

    size_t len;
    எனில் (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } இல்லையெனில் {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == வெற்று;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    எனில் (str == வெற்று) {
        _Pypegen_raise_decode_error(p);
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Constant(str, வெற்று, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

நிலையான asdl_கோவை_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_கோவை_seq *raw_expressions,
                   Token *b, முழு is_tstring)
{
    Py_ssize_t n_பொருட்கள் = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_பொருட்கள் = n_பொருட்கள்;
    க்கு (Py_ssize_t i = 0; i < n_பொருட்கள்; i++) {
        கோவை_ty item = asdl_seq_GET(raw_expressions, i);
        எனில் (item->kind == JoinedStr_kind) {
            total_பொருட்கள் += asdl_seq_LEN(item->v.JoinedStr.மதிப்புகள்) - 1;
        }
    }

    மாறிலி எழுத்து* quote_str = PyBytes_AsString(a->bytes);
    எனில் (quote_str == வெற்று) {
        திரும்பு வெற்று;
    }
    முழு is_raw = strpbrk(quote_str, "rR") != வெற்று;

    asdl_கோவை_seq *seq = Py_asdl_கோவை_seq_new(total_பொருட்கள், p->arena);
    எனில் (seq == வெற்று) {
        திரும்பு வெற்று;
    }

    Py_ssize_t index = 0;
    க்கு (Py_ssize_t i = 0; i < n_பொருட்கள்; i++) {
        கோவை_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        எனில் (item->kind == JoinedStr_kind) {
            asdl_கோவை_seq *மதிப்புகள் = item->v.JoinedStr.மதிப்புகள்;
            எனில் (asdl_seq_LEN(மதிப்புகள்) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                திரும்பு வெற்று;
            }

            கோவை_ty first = asdl_seq_GET(மதிப்புகள், 0);
            ப்ரதிஜ்ஞா(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            கோவை_ty second = asdl_seq_GET(மதிப்புகள், 1);
            ப்ரதிஜ்ஞா((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_மதிப்பு_kind);
            asdl_seq_SET(seq, index++, second);

            தொடர்;
        }

        எனில் (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            எனில் (item == வெற்று) {
                திரும்பு வெற்று;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            எனில் (PyUnicode_CheckExact(item->v.Constant.மதிப்பு)
                && PyUnicode_GET_LENGTH(item->v.Constant.மதிப்பு) == 0) {
                தொடர்;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_கோவை_seq *resized_exprs;
    எனில் (index != total_பொருட்கள்) {
        resized_exprs = Py_asdl_கோவை_seq_new(index, p->arena);
        எனில் (resized_exprs == வெற்று) {
            திரும்பு வெற்று;
        }
        க்கு (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    இல்லையெனில் {
        resized_exprs = seq;
    }
    திரும்பு resized_exprs;
}

கோவை_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_கோவை_seq *raw_expressions, Token *b) {

    asdl_கோவை_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    எனில் (resized_exprs == வெற்று) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

கோவை_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_கோவை_seq* raw_expressions, Token*b) {

    asdl_கோவை_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    எனில் (resized_exprs == வெற்று) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

கோவை_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    எனில் (tok->bytes == வெற்று) {
        திரும்பு Py_Pegen_raise_incomplete_உள்ளீடு(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    மாறிலி எழுத்து *bstr = PyBytes_AS_STRING(tok->bytes);

    முழு is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    எனில் (str == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Constant(str, வெற்று, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

கோவை_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    எனில் (tok->bytes == வெற்று) {
        திரும்பு Py_Pegen_raise_incomplete_உள்ளீடு(p);
    }
    மாறிலி எழுத்து *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    எனில் (str == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Constant(str, வெற்று, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

கோவை_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    எனில் (tok->bytes == வெற்று) {
        திரும்பு Py_Pegen_raise_incomplete_உள்ளீடு(p);
    }
    மாறிலி எழுத்து *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    எனில் (s == வெற்று) {
        _Pypegen_raise_decode_error(p);
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        திரும்பு வெற்று;
    }
    PyObject *kind = வெற்று;
    எனில் (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        எனில் (kind == வெற்று) {
            திரும்பு வெற்று;
        }
    }
    திரும்பு _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

நிலையான முழு
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    எனில் (conversion != வெற்று) {
        கோவை_ty conversion_கோவை = (கோவை_ty) conversion->result;
        ப்ரதிஜ்ஞா(conversion_கோவை->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_கோவை->v.Name.id, 0);
        திரும்பு Py_SAFE_DOWNCAST(first, Py_UCS4, முழு);
    }
    இல்லையெனில் எனில் (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        திரும்பு (முழு)'r';
    }
    திரும்பு -1;
}

நிலையான PyObject *
strip_interpolation_debug_கோவை(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    போது (len > 0) {
        முழு has_newline = 0;
        போது (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            எனில் (!_PyUnicode_IsWhitespace(c)) {
                நிறுத்து;
            }
            எனில் (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        எனில் (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            நிறுத்து;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    எனில் (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        திரும்பு Py_NewRef(exprstr);
    }

    திரும்பு PyUnicode_Substring(exprstr, 0, len - 1);
}

கோவை_ty _PyPegen_interpolation(Parser *p, கோவை_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, முழு lineno, முழு col_offset,
                                 முழு end_lineno, முழு end_col_offset, PyArena *arena) {

    முழு conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    முழு debug_end_வரி, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    எனில் (conversion) {
        debug_end_வரி = ((கோவை_ty) conversion->result)->lineno;
        debug_end_offset = ((கோவை_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    இல்லையெனில் எனில் (format) {
        debug_end_வரி = ((கோவை_ty) format->result)->lineno;
        debug_end_offset = ((கோவை_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    இல்லையெனில் {
        debug_end_வரி = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ப்ரதிஜ்ஞா(exprstr != வெற்று);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_கோவை(exprstr)
        : Py_NewRef(exprstr);
    எனில் (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        திரும்பு வெற்று;
    }

    கோவை_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (கோவை_ty) format->result : வெற்று,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    எனில் (!interpolation || !debug) {
        திரும்பு interpolation;
    }

    கோவை_ty debug_text = _PyAST_Constant(debug_metadata, வெற்று, lineno, col_offset + 1, debug_end_வரி,
                                            debug_end_offset - 1, p->arena);
    எனில் (!debug_text) {
        திரும்பு வெற்று;
    }

    asdl_கோவை_seq *மதிப்புகள் = Py_asdl_கோவை_seq_new(2, arena);
    எனில் (!மதிப்புகள்) {
        திரும்பு வெற்று;
    }
    asdl_seq_SET(மதிப்புகள், 0, debug_text);
    asdl_seq_SET(மதிப்புகள், 1, interpolation);
    திரும்பு _PyAST_JoinedStr(மதிப்புகள், lineno, col_offset, debug_end_வரி, debug_end_offset, p->arena);
}

கோவை_ty Py_Pegen_formatted_மதிப்பு(Parser *p, கோவை_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, முழு lineno, முழு col_offset,
                                 முழு end_lineno, முழு end_col_offset, PyArena *arena) {
    முழு conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    கோவை_ty formatted_மதிப்பு = Py_AST_Formatted_மதிப்பு(
        expression, conversion_val, format ? (கோவை_ty) format->result : வெற்று,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    எனில் (!formatted_மதிப்பு || !debug) {
        திரும்பு formatted_மதிப்பு;
    }

    /* Find the non whitespace token after the "=" */
    முழு debug_end_வரி, debug_end_offset;
    PyObject *debug_metadata;

    எனில் (conversion) {
        debug_end_வரி = ((கோவை_ty) conversion->result)->lineno;
        debug_end_offset = ((கோவை_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    இல்லையெனில் எனில் (format) {
        debug_end_வரி = ((கோவை_ty) format->result)->lineno;
        debug_end_offset = ((கோவை_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    இல்லையெனில் {
        debug_end_வரி = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    கோவை_ty debug_text = _PyAST_Constant(debug_metadata, வெற்று, lineno, col_offset + 1, debug_end_வரி,
                                            debug_end_offset - 1, p->arena);
    எனில் (!debug_text) {
        திரும்பு வெற்று;
    }

    asdl_கோவை_seq *மதிப்புகள் = Py_asdl_கோவை_seq_new(2, arena);
    எனில் (!மதிப்புகள்) {
        திரும்பு வெற்று;
    }
    asdl_seq_SET(மதிப்புகள், 0, debug_text);
    asdl_seq_SET(மதிப்புகள், 1, formatted_மதிப்பு);
    திரும்பு _PyAST_JoinedStr(மதிப்புகள், lineno, col_offset, debug_end_வரி, debug_end_offset, p->arena);
}

நிலையான கோவை_ty
_build_concatenated_bytes(Parser *p, asdl_கோவை_seq *strings, முழு lineno,
                        முழு col_offset, முழு end_lineno, முழு end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ப்ரதிஜ்ஞா(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.மதிப்பு;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        எனில் (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            திரும்பு வெற்று;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    எனில் (writer == வெற்று) {
        திரும்பு வெற்று;
    }
    எழுத்து *out = PyBytesWriter_GetData(writer);

    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.மதிப்பு;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        எனில் (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    எனில் (res == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

நிலையான கோவை_ty
_build_concatenated_unicode(Parser *p, asdl_கோவை_seq *strings, முழு lineno,
                        முழு col_offset, முழு end_lineno, முழு end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ப்ரதிஜ்ஞா(len > 1);

    கோவை_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    எனில் (writer == வெற்று) {
        திரும்பு வெற்று;
    }

    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty current_elem = asdl_seq_GET(strings, i);
        ப்ரதிஜ்ஞா(current_elem->kind == Constant_kind);

        எனில் (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.மதிப்பு)) {
            PyUnicodeWriter_Discard(writer);
            திரும்பு வெற்று;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    எனில் (final == வெற்று) {
        திரும்பு வெற்று;
    }
    எனில் (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

நிலையான asdl_கோவை_seq *
_build_concatenated_str(Parser *p, asdl_கோவை_seq *strings,
                               முழு lineno, முழு col_offset, முழு end_lineno,
                               முழு end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ப்ரதிஜ்ஞா(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty elem = asdl_seq_GET(strings, i);
        தேர்வு(elem->kind) {
            நிலை JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.மதிப்புகள்);
                நிறுத்து;
            நிலை TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.மதிப்புகள்);
                நிறுத்து;
            இயல்பு:
                n_flattened_elements++;
                நிறுத்து;
        }
    }


    asdl_கோவை_seq* flattened = Py_asdl_கோவை_seq_new(n_flattened_elements, p->arena);
    எனில் (flattened == வெற்று) {
        திரும்பு வெற்று;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    க்கு (Py_ssize_t i = 0; i < len; i++) {
        கோவை_ty elem = asdl_seq_GET(strings, i);
        தேர்வு(elem->kind) {
            நிலை JoinedStr_kind:
                க்கு (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.மதிப்புகள்); j++) {
                    கோவை_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.மதிப்புகள், j);
                    எனில் (subvalue == வெற்று) {
                        திரும்பு வெற்று;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                நிறுத்து;
            நிலை TemplateStr_kind:
                க்கு (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.மதிப்புகள்); j++) {
                    கோவை_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.மதிப்புகள், j);
                    எனில் (subvalue == வெற்று) {
                        திரும்பு வெற்று;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                நிறுத்து;
            இயல்பு:
                asdl_seq_SET(flattened, current_pos++, elem);
                நிறுத்து;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    முழு prev_is_constant = 0;
    க்கு (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        கோவை_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        எனில் (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.மதிப்பு) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.மதிப்பு) == 0)
            தொடர்;

        எனில் (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_கோவை_seq* மதிப்புகள் = Py_asdl_கோவை_seq_new(n_elements, p->arena);
    எனில் (மதிப்புகள் == வெற்று) {
        திரும்பு வெற்று;
    }

    /* build folded list */
    current_pos = 0;
    க்கு (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        கோவை_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        எனில் (elem->kind == Constant_kind) {
            எனில் (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                கோவை_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                எனில் (writer == வெற்று) {
                    திரும்பு வெற்று;
                }
                கோவை_ty last_elem = elem;
                Py_ssize_t j;
                க்கு (j = i; j < n_flattened_elements; j++) {
                    கோவை_ty current_elem = asdl_seq_GET(flattened, j);
                    எனில் (current_elem->kind == Constant_kind) {
                        எனில் (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.மதிப்பு)) {
                            PyUnicodeWriter_Discard(writer);
                            திரும்பு வெற்று;
                        }
                        last_elem = current_elem;
                    } இல்லையெனில் {
                        நிறுத்து;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                எனில் (concat_str == வெற்று) {
                    திரும்பு வெற்று;
                }
                எனில் (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    திரும்பு வெற்று;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                எனில் (elem == வெற்று) {
                    திரும்பு வெற்று;
                }
            }

            /* Drop all empty contanst strings */
            எனில் (PyUnicode_CheckExact(elem->v.Constant.மதிப்பு) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.மதிப்பு) == 0) {
                தொடர்;
            }
        }

        asdl_seq_SET(மதிப்புகள், current_pos++, elem);
    }

    ப்ரதிஜ்ஞா(current_pos == n_elements);
    திரும்பு மதிப்புகள்;
}

நிலையான கோவை_ty
_build_concatenated_joined_str(Parser *p, asdl_கோவை_seq *strings,
                               முழு lineno, முழு col_offset, முழு end_lineno,
                               முழு end_col_offset, PyArena *arena)
{
    asdl_கோவை_seq *மதிப்புகள் = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    எனில் (!மதிப்புகள்) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_JoinedStr(மதிப்புகள், lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

கோவை_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_கோவை_seq *strings,
                               முழு lineno, முழு col_offset, முழு end_lineno,
                               முழு end_col_offset, PyArena *arena)
{
    asdl_கோவை_seq *மதிப்புகள் = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    எனில் (!மதிப்புகள்) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyAST_TemplateStr(மதிப்புகள், lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

கோவை_ty
_PyPegen_concatenate_strings(Parser *p, asdl_கோவை_seq *strings,
                             முழு lineno, முழு col_offset, முழு end_lineno,
                             முழு end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    ப்ரதிஜ்ஞா(len > 0);

    முழு f_string_found = 0;
    முழு unicode_string_found = 0;
    முழு bytes_found = 0;

    Py_ssize_t i = 0;
    க்கு (i = 0; i < len; i++) {
        கோவை_ty elem = asdl_seq_GET(strings, i);
        தேர்வு(elem->kind) {
            நிலை Constant_kind:
                எனில் (PyBytes_CheckExact(elem->v.Constant.மதிப்பு)) {
                    bytes_found = 1;
                } இல்லையெனில் {
                    unicode_string_found = 1;
                }
                நிறுத்து;
            நிலை JoinedStr_kind:
                f_string_found = 1;
                நிறுத்து;
            நிலை TemplateStr_kind:
                // python.gram handles this; we should never get here
                ப்ரதிஜ்ஞா(0);
                நிறுத்து;
            இயல்பு:
                f_string_found = 1;
                நிறுத்து;
        }
    }

    // Cannot mix unicode and bytes
    எனில் ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        திரும்பு வெற்று;
    }

    // If it's only bytes or only unicode string, do a simple concat
    எனில் (!f_string_found) {
        எனில் (len == 1) {
            திரும்பு asdl_seq_GET(strings, 0);
        }
        இல்லையெனில் எனில் (bytes_found) {
            திரும்பு _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        இல்லையெனில் {
            திரும்பு _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    திரும்பு _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

நிலையான முழு
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, கோவை_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    எனில் (p->call_invalid_rules) {
        திரும்பு 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    எனில் (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        திரும்பு 0;
    }

    முழு மொத்தம் = Py_Pegen_seq_மொத்தம்_dots(dots);
    எழுத்து *buf = PyMem_RawMalloc(மொத்தம் + 1);
    எனில் (buf == வெற்று) {
        PyErr_NoMemory();
        திரும்பு -1;
    }
    memset(buf, '.', மொத்தம்);
    buf[மொத்தம்] = '\0';

    PyObject *செய்தி = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    எனில் (செய்தி == வெற்று) {
        திரும்பு -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    முழு res = _PyErr_EmitSyntaxWarning(செய்தி,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(செய்தி);
    திரும்பு res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, கோவை_ty module_name,
                             asdl_alias_seq *names, கோவை_ty lazy_token, முழு lineno,
                             முழு col_offset, முழு end_lineno, முழு end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    முழு level = Py_Pegen_seq_மொத்தம்_dots(dots);
    எனில் (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        எனில் (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            திரும்பு வெற்று;
        }
        க்கு (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            எனில் (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    இல்லையெனில் எனில் (
        level > 0
        && lazy_token == வெற்று
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        எனில் (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            திரும்பு வெற்று;
        }
    }
    திரும்பு _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    எனில் (!p->call_invalid_rules) {
        திரும்பு stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    எனில் (len == 0) {
        திரும்பு stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    எனில் (p->last_stmt_location.lineno > last_stmt->lineno) {
        திரும்பு stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    திரும்பு stmts;
}
