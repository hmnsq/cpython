#شامل <Python.h>
#شامل "pycore_pystate.h"         // _PyInterpreterState_GET()
#شامل "pycore_runtime.h"         // _PyRuntime
#شامل "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#شامل "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#شامل "pegen.h"
#شامل "string_parser.h"          // _PyPegen_decode_string()
#شامل "lexer/state.h"            // tok_state


تهی *
_PyPegen_dummy_name(Parser *p, ...)
{
    بازگشت &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, تهی *a)
{
    أكد(a != پوچ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    اگر (!seq) {
        بازگشت پوچ;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    بازگشت seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_درج_in_front(Parser *p, تهی *a, asdl_seq *seq)
{
    أكد(a != پوچ);
    اگر (!seq) {
        بازگشت _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    برای (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    بازگشت new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, تهی *a)
{
    أكد(a != پوچ);
    اگر (!seq) {
        بازگشت _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }

    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    بازگشت new_seq;
}

ایستا Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    بازگشت size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    أكد(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    اگر (!flattened_seq) {
        بازگشت پوچ;
    }

    صحیح flattened_seq_idx = 0;
    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        برای (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_size);

    بازگشت flattened_seq;
}

تهی *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    بازگشت asdl_seq_GET_UNTYPED(seq, len - 1);
}

تهی *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    بازگشت asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
عبارت_ty
_PyPegen_join_names_with_dot(Parser *p, عبارت_ty first_name, عبارت_ty second_name)
{
    أكد(first_name != پوچ && second_name != پوچ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    اگر (!uni) {
        بازگشت پوچ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    اگر (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        بازگشت پوچ;
    }

    بازگشت _PyAST_Name(uni, Load, EXTRA_عبارت(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
صحیح
Py_Pegen_seq_شمار_dots(asdl_seq *seq)
{
    صحیح عدد_of_dots = 0;
    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_عبارت = asdl_seq_GET_UNTYPED(seq, i);
        انتخاب (current_عبارت->type) {
            حالت ELLIPSIS:
                عدد_of_dots += 3;
                بشکن;
            حالت DOT:
                عدد_of_dots += 1;
                بشکن;
            پیشفرض:
                Py_UNREACHABLE();
        }
    }

    بازگشت عدد_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, صحیح lineno, صحیح col_offset, صحیح end_lineno,
                        صحیح end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    اگر (!str) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        بازگشت پوچ;
    }
    بازگشت _PyAST_alias(str, پوچ, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_عبارت_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    بازگشت new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_عبارت_Pair *
Py_Pegen_cmpop_عبارت_pair(Parser *p, cmpop_ty cmpop, عبارت_ty عبارت)
{
    أكد(عبارت != پوچ);
    Cmpop_عبارت_Pair *a = _PyArena_Malloc(p->arena, اندازه(Cmpop_عبارت_Pair));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->cmpop = cmpop;
    a->عبارت = عبارت;
    بازگشت a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_عبارت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    بازگشت new_seq;
}

asdl_عبارت_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_عبارت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->عبارت);
    }
    بازگشت new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ایستا asdl_عبارت_seq *
_set_seq_context(Parser *p, asdl_عبارت_seq *seq, عبارت_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    اگر (len == 0) {
        بازگشت پوچ;
    }

    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty e = asdl_seq_GET(seq, i);
        عبارت_ty new_e = Py_Pegen_set_عبارت_context(p, e, ctx);
        اگر (!new_e) {
            بازگشت پوچ;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    بازگشت new_seq;
}

ایستا عبارت_ty
_set_name_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    بازگشت _PyAST_Name(e->v.Name.id, ctx, EXTRA_عبارت(e, e));
}

ایستا عبارت_ty
_set_tuple_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    asdl_عبارت_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    اگر (!seq && PyErr_Occurred()) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_Tuple(seq, ctx, EXTRA_عبارت(e, e));
}

ایستا عبارت_ty
_set_list_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    asdl_عبارت_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    اگر (!seq && PyErr_Occurred()) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_List(seq, ctx, EXTRA_عبارت(e, e));
}

ایستا عبارت_ty
_set_subscript_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    بازگشت _PyAST_Subscript(e->v.Subscript.مقدار, e->v.Subscript.slice,
                            ctx, EXTRA_عبارت(e, e));
}

ایستا عبارت_ty
_set_attribute_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    بازگشت _PyAST_Attribute(e->v.Attribute.مقدار, e->v.Attribute.attr,
                            ctx, EXTRA_عبارت(e, e));
}

ایستا عبارت_ty
_set_starred_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    عبارت_ty inner = Py_Pegen_set_عبارت_context(p, e->v.Starred.مقدار, ctx);
    اگر (!inner) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_Starred(inner, ctx, EXTRA_عبارت(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
عبارت_ty
Py_Pegen_set_عبارت_context(Parser *p, عبارت_ty عبارت, عبارت_context_ty ctx)
{
    أكد(عبارت != پوچ);

    عبارت_ty new = پوچ;
    انتخاب (عبارت->kind) {
        حالت Name_kind:
            new = _set_name_context(p, عبارت, ctx);
            بشکن;
        حالت Tuple_kind:
            new = _set_tuple_context(p, عبارت, ctx);
            بشکن;
        حالت List_kind:
            new = _set_list_context(p, عبارت, ctx);
            بشکن;
        حالت Subscript_kind:
            new = _set_subscript_context(p, عبارت, ctx);
            بشکن;
        حالت Attribute_kind:
            new = _set_attribute_context(p, عبارت, ctx);
            بشکن;
        حالت Starred_kind:
            new = _set_starred_context(p, عبارت, ctx);
            بشکن;
        پیشفرض:
            new = عبارت;
    }
    بازگشت new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
عبارت_ty
_PyPegen_make_target(Parser *p, عبارت_ty عبارت, TARGETS_TYPE targets_type)
{
    أكد(عبارت != پوچ);
    أكد(targets_type != FOR_TARGETS);
    اگر (_PyPegen_get_invalid_target(عبارت, targets_type) != پوچ) {
        بازگشت پوچ;
    }
    بازگشت Py_Pegen_set_عبارت_context(
        p, عبارت, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
کلید_مقدار_Pair *
Py_Pegen_کلید_مقدار_pair(Parser *p, عبارت_ty کلید, عبارت_ty مقدار)
{
    کلید_مقدار_Pair *a = _PyArena_Malloc(p->arena, اندازه(کلید_مقدار_Pair));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->کلید = کلید;
    a->مقدار = مقدار;
    بازگشت a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_عبارت_seq *
Py_Pegen_get_کلیدها(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        کلید_مقدار_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->کلید);
    }
    بازگشت new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_عبارت_seq *
Py_Pegen_get_مقادیر(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        کلید_مقدار_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->مقدار);
    }
    بازگشت new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
کلید_Pattern_Pair *
Py_Pegen_کلید_pattern_pair(Parser *p, عبارت_ty کلید, pattern_ty pattern)
{
    کلید_Pattern_Pair *a = _PyArena_Malloc(p->arena, اندازه(کلید_Pattern_Pair));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->کلید = کلید;
    a->pattern = pattern;
    بازگشت a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_عبارت_seq *
Py_Pegen_get_pattern_کلیدها(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        کلید_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->کلید);
    }
    بازگشت new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        کلید_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    بازگشت new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, عبارت_ty مقدار, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, اندازه(NameDefaultPair));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    اگر (!a->arg) {
        بازگشت پوچ;
    }
    a->مقدار = مقدار;
    بازگشت a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, اندازه(SlashWithDefault));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    بازگشت a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, اندازه(StarEtc));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    بازگشت a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }

    صحیح k = 0;
    برای (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    برای (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    بازگشت new_seq;
}

ایستا asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    اگر (!seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    بازگشت seq;
}

ایستا asdl_عبارت_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_عبارت_seq *seq = Py_asdl_عبارت_seq_new(len, p->arena);
    اگر (!seq) {
        بازگشت پوچ;
    }
    برای (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->مقدار);
    }
    بازگشت seq;
}

ایستا صحیح
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    اگر (slash_without_default != پوچ) {
        *posonlyargs = slash_without_default;
    }
    وگرنه اگر (slash_with_default != پوچ) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        اگر (!slash_with_default_names) {
            بازگشت -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    وگرنه {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    بازگشت *posonlyargs == پوچ ? -1 : 0;
}

ایستا صحیح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    اگر (names_with_default != پوچ) {
        اگر (plain_names != پوچ) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            اگر (!names_with_default_names) {
                بازگشت -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        وگرنه {
            *posargs = _get_names(p, names_with_default);
        }
    }
    وگرنه {
        اگر (plain_names != پوچ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_names;
        }
        وگرنه {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    بازگشت *posargs == پوچ ? -1 : 0;
}

ایستا صحیح
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_عبارت_seq **posdefaults) {
    اگر (slash_with_default != پوچ && names_with_default != پوچ) {
        asdl_عبارت_seq *slash_with_default_مقادیر =
                _get_defaults(p, slash_with_default->names_with_defaults);
        اگر (!slash_with_default_مقادیر) {
            بازگشت -1;
        }
        asdl_عبارت_seq *names_with_default_مقادیر = _get_defaults(p, names_with_default);
        اگر (!names_with_default_مقادیر) {
            بازگشت -1;
        }
        *posdefaults = (asdl_عبارت_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_مقادیر,
                (asdl_seq*)names_with_default_مقادیر);
    }
    وگرنه اگر (slash_with_default == پوچ && names_with_default != پوچ) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    وگرنه اگر (slash_with_default != پوچ && names_with_default == پوچ) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    وگرنه {
        *posdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    }
    بازگشت *posdefaults == پوچ ? -1 : 0;
}

ایستا صحیح
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_عبارت_seq **kwdefaults) {
    اگر (star_etc != پوچ && star_etc->kwonlyargs != پوچ) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    وگرنه {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    اگر (*kwonlyargs == پوچ) {
        بازگشت -1;
    }

    اگر (star_etc != پوچ && star_etc->kwonlyargs != پوچ) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    وگرنه {
        *kwdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    }

    اگر (*kwdefaults == پوچ) {
        بازگشت -1;
    }

    بازگشت 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    اگر (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        بازگشت پوچ;
    }

    asdl_arg_seq *posargs;
    اگر (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        بازگشت پوچ;
    }

    asdl_عبارت_seq *posdefaults;
    اگر (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        بازگشت پوچ;
    }

    arg_ty vararg = پوچ;
    اگر (star_etc != پوچ && star_etc->vararg != پوچ) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_عبارت_seq *kwdefaults;
    اگر (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        بازگشت پوچ;
    }

    arg_ty kwarg = پوچ;
    اگر (star_etc != پوچ && star_etc->kwarg != پوچ) {
        kwarg = star_etc->kwarg;
    }

    بازگشت _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!posonlyargs) {
        بازگشت پوچ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!posargs) {
        بازگشت پوچ;
    }
    asdl_عبارت_seq *posdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    اگر (!posdefaults) {
        بازگشت پوچ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!kwonlyargs) {
        بازگشت پوچ;
    }
    asdl_عبارت_seq *kwdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    اگر (!kwdefaults) {
        بازگشت پوچ;
    }

    بازگشت _PyAST_arguments(posonlyargs, posargs, پوچ, kwonlyargs,
                            kwdefaults, پوچ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, اندازه(AugOperator));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->kind = kind;
    بازگشت a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_عبارت_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != پوچ);
    اگر (function_def->kind == AsyncFunctionDef_kind) {
        بازگشت _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    بازگشت _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_عبارت_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != پوچ);
    بازگشت _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, تهی *element, صحیح is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, اندازه(KeywordOrStarred));
    اگر (!a) {
        بازگشت پوچ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    بازگشت a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ایستا صحیح
seq_عدد_of_starred_exprs(asdl_seq *seq)
{
    صحیح n = 0;
    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        اگر (!k->is_keyword) {
            n++;
        }
    }
    بازگشت n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_عبارت_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحیح new_len = seq_عدد_of_starred_exprs(kwargs);
    اگر (new_len == 0) {
        بازگشت پوچ;
    }
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(new_len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }

    صحیح idx = 0;
    برای (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        اگر (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    بازگشت new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_عدد_of_starred_exprs(kwargs);
    اگر (new_len == 0) {
        بازگشت پوچ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    اگر (!new_seq) {
        بازگشت پوچ;
    }

    صحیح idx = 0;
    برای (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        اگر (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    بازگشت new_seq;
}

عبارت_ty
_PyPegen_ensure_imaginary(Parser *p, عبارت_ty exp)
{
    اگر (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.مقدار)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        بازگشت پوچ;
    }
    بازگشت exp;
}

عبارت_ty
_PyPegen_ensure_real(Parser *p, عبارت_ty exp)
{
    اگر (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.مقدار)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        بازگشت پوچ;
    }
    بازگشت exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = پوچ;
    Py_ssize_t رقم = p->type_ignore_comments.رقم_اقلام;
    اگر (رقم > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(رقم, p->arena);
        اگر (type_ignores == پوچ) {
            بازگشت پوچ;
        }
        برای (Py_ssize_t i = 0; i < رقم; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.اقلام[i].comment);
            اگر (tag == پوچ) {
                بازگشت پوچ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.اقلام[i].lineno,
                                                  tag, p->arena);
            اگر (ti == پوچ) {
                بازگشت پوچ;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    بازگشت _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت نویسه *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), پوچ);
    اگر (res == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        بازگشت پوچ;
    }
    بازگشت res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    اگر (tc == پوچ) {
        بازگشت a;
    }
    ثابت نویسه *bytes = PyBytes_AsString(tc->bytes);
    اگر (bytes == پوچ) {
        بازگشت پوچ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    اگر (tco == پوچ) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحیح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->bytes != پوچ);
    أكد(t->type == NOTEQUAL);

    ثابت نویسه* tok_str = PyBytes_AS_STRING(t->bytes);
    اگر (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        بازگشت -1;
    }
    اگر (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        بازگشت strcmp(tok_str, "!=");
    }
    بازگشت 0;
}

صحیح
_PyPegen_check_legacy_stmt(Parser *p, عبارت_ty name) {
    اگر (name->kind != Name_kind) {
        بازگشت 0;
    }
    ثابت نویسه* candidates[2] = {"print", "exec"};
    برای (صحیح i=0; i<2; i++) {
        اگر (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            بازگشت 1;
        }
    }
    بازگشت 0;
}

تهی *
_PyPegen_raise_error_for_missing_comma(Parser *p, عبارت_ty a, عبارت_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    اگر (_PyPegen_check_legacy_stmt(p, a)) {
        بازگشت پوچ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    اگر (p->tokens[p->mark - 1]->level == 0) {
        بازگشت پوچ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    اگر (a->end_lineno > a->lineno) {
        بازگشت RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    بازگشت RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ایستا ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, تهی *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, اندازه(ResultTokenWithMetadata));
    اگر (res == پوچ) {
        بازگشت پوچ;
    }
    res->metadata = metadata;
    res->result = result;
    بازگشت res;
}

ایستا نویسه
formatted_string_prefix(ثابت Parser *p)
{
    صحیح nested = 0;
    برای (صحیح i = p->mark - 1; i >= 0; i--) {
        صحیح type = p->tokens[i]->type;
        اگر (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        وگرنه اگر (type == FSTRING_START || type == TSTRING_START) {
            اگر (nested == 0) {
                بازگشت type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, عبارت_ty conv)
{
    اگر (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        بازگشت RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    اگر (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        بازگشت پوچ;
    }

    بازگشت result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_عبارت_seq *spec, صحیح lineno, صحیح col_offset,
                                صحیح end_lineno, صحیح end_col_offset, PyArena *arena)
{
    اگر (!spec) {
        بازگشت پوچ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_اقلام = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_شمار = 0;
    برای (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty item = asdl_seq_GET(spec, i);
        non_empty_شمار += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.مقدار) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.مقدار) == 0);
    }
    اگر (non_empty_شمار != n_اقلام) {
        asdl_عبارت_seq *resized_spec =
            Py_asdl_عبارت_seq_new(non_empty_شمار, p->arena);
        اگر (resized_spec == پوچ) {
            بازگشت پوچ;
        }
        Py_ssize_t j = 0;
        برای (Py_ssize_t i = 0; i < n_اقلام; i++) {
            عبارت_ty item = asdl_seq_GET(spec, i);
            اگر (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.مقدار) &&
                PyUnicode_GET_LENGTH(item->v.Constant.مقدار) == 0) {
                ادامه;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        أكد(j == non_empty_شمار);
        spec = resized_spec;
    }
    عبارت_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    اگر (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } وگرنه {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    اگر (!res) {
        بازگشت پوچ;
    }
    بازگشت result_token_with_metadata(p, res, colon->metadata);
}

ثابت نویسه *
Py_Pegen_get_عبارت_name(عبارت_ty e)
{
    أكد(e != پوچ);
    انتخاب (e->kind) {
        حالت Attribute_kind:
            بازگشت "attribute";
        حالت Subscript_kind:
            بازگشت "subscript";
        حالت Starred_kind:
            بازگشت "starred";
        حالت Name_kind:
            بازگشت "name";
        حالت List_kind:
            بازگشت "list";
        حالت Tuple_kind:
            بازگشت "tuple";
        حالت Lambda_kind:
            بازگشت "lambda";
        حالت Call_kind:
            بازگشت "function call";
        حالت BoolOp_kind:
        حالت BinOp_kind:
        حالت UnaryOp_kind:
            بازگشت "expression";
        حالت GeneratorExp_kind:
            بازگشت "generator expression";
        حالت Yield_kind:
        حالت YieldFrom_kind:
            بازگشت "yield expression";
        حالت Await_kind:
            بازگشت "await expression";
        حالت ListComp_kind:
            بازگشت "list comprehension";
        حالت SetComp_kind:
            بازگشت "set comprehension";
        حالت DictComp_kind:
            بازگشت "dict comprehension";
        حالت Dict_kind:
            بازگشت "dict literal";
        حالت Set_kind:
            بازگشت "set display";
        حالت JoinedStr_kind:
        حالت Formatted_مقدار_kind:
            بازگشت "f-string expression";
        حالت TemplateStr_kind:
        حالت Interpolation_kind:
            بازگشت "t-string expression";
        حالت Constant_kind: {
            PyObject *مقدار = e->v.Constant.مقدار;
            اگر (مقدار == Py_None) {
                بازگشت "None";
            }
            اگر (مقدار == Py_False) {
                بازگشت "False";
            }
            اگر (مقدار == Py_True) {
                بازگشت "True";
            }
            اگر (مقدار == Py_Ellipsis) {
                بازگشت "ellipsis";
            }
            بازگشت "literal";
        }
        حالت Compare_kind:
            بازگشت "comparison";
        حالت IfExp_kind:
            بازگشت "conditional expression";
        حالت Named_عبارت_kind:
            بازگشت "named expression";
        پیشفرض:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            بازگشت پوچ;
    }
}

عبارت_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    اگر (comprehension->ifs == پوچ || asdl_seq_LEN(comprehension->ifs) == 0) {
        بازگشت comprehension->iter;
    }
    بازگشت PyPegen_last_item(comprehension->ifs, عبارت_ty);
}

عبارت_ty _PyPegen_collect_call_seqs(Parser *p, asdl_عبارت_seq *a, asdl_seq *b,
                     صحیح lineno, صحیح col_offset, صحیح end_lineno,
                     صحیح end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    اگر (b == پوچ) {
        بازگشت _PyAST_Call(_PyPegen_dummy_name(p), a, پوچ, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_عبارت_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    اگر (!starreds && PyErr_Occurred()) {
        بازگشت پوچ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    اگر (!keywords && PyErr_Occurred()) {
        بازگشت پوچ;
    }

    اگر (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_عبارت_seq *args = Py_asdl_عبارت_seq_new(total_len, arena);
    اگر (args == پوچ) {
        بازگشت پوچ;
    }

    Py_ssize_t i = 0;
    برای (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    برای (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    بازگشت _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

عبارت_ty
_PyPegen_get_invalid_target(عبارت_ty e, TARGETS_TYPE targets_type)
{
    اگر (e == پوچ) {
        بازگشت پوچ;
    }

    اگر (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        اگر (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            بازگشت پوچ;
        }
        بازگشت e->kind == Attribute_kind || e->kind == Subscript_kind ? پوچ : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) انجامبده { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        برای (Py_ssize_t i = 0; i < len; i++) {\
            عبارت_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            عبارت_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            اگر (child != پوچ) {\
                بازگشت child;\
            }\
        }\
    } وقتی (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    انتخاب (e->kind) {
        حالت List_kind:
            VISIT_CONTAINER(e, List);
            بازگشت پوچ;
        حالت Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            بازگشت پوچ;
        حالت Starred_kind:
            اگر (targets_type == DEL_TARGETS) {
                بازگشت e;
            }
            بازگشت _PyPegen_get_invalid_target(e->v.Starred.مقدار, targets_type);
        حالت Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            اگر (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                اگر (cmpop == In) {
                    بازگشت _PyPegen_get_invalid_target(e->v.Compare.چپ, targets_type);
                }
                بازگشت پوچ;
            }
            بازگشت e;
        حالت Name_kind:
        حالت Subscript_kind:
        حالت Attribute_kind:
            بازگشت پوچ;
        پیشفرض:
            بازگشت e;
    }
}

تهی *_PyPegen_arguments_parsing_error(Parser *p, عبارت_ty e) {
    صحیح kwarg_unpacking = 0;
    برای (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        اگر (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت نویسه *پیام = پوچ;
    اگر (kwarg_unpacking) {
        پیام = "positional argument follows keyword argument unpacking";
    } وگرنه {
        پیام = "positional argument follows keyword argument";
    }

    بازگشت RAISE_SYNTAX_ERROR(پیام);
}

تهی *
_PyPegen_nonparen_genexp_in_call(Parser *p, عبارت_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    اگر (len <= 1) {
        بازگشت پوچ;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    بازگشت RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (عبارت_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ایستا عبارت_ty
_PyPegen_decode_fstring_part(Parser* p, صحیح is_raw, عبارت_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.مقدار));

    ثابت نویسه* bstr = PyUnicode_AsUTF8(constant->v.Constant.مقدار);
    اگر (bstr == پوچ) {
        بازگشت پوچ;
    }

    size_t len;
    اگر (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } وگرنه {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == پوچ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    اگر (str == پوچ) {
        _Pypegen_raise_decode_error(p);
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        بازگشت پوچ;
    }
    بازگشت _PyAST_Constant(str, پوچ, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ایستا asdl_عبارت_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions,
                   Token *b, صحیح is_tstring)
{
    Py_ssize_t n_اقلام = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_اقلام = n_اقلام;
    برای (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty item = asdl_seq_GET(raw_expressions, i);
        اگر (item->kind == JoinedStr_kind) {
            total_اقلام += asdl_seq_LEN(item->v.JoinedStr.مقادیر) - 1;
        }
    }

    ثابت نویسه* quote_str = PyBytes_AsString(a->bytes);
    اگر (quote_str == پوچ) {
        بازگشت پوچ;
    }
    صحیح is_raw = strpbrk(quote_str, "rR") != پوچ;

    asdl_عبارت_seq *seq = Py_asdl_عبارت_seq_new(total_اقلام, p->arena);
    اگر (seq == پوچ) {
        بازگشت پوچ;
    }

    Py_ssize_t index = 0;
    برای (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        اگر (item->kind == JoinedStr_kind) {
            asdl_عبارت_seq *مقادیر = item->v.JoinedStr.مقادیر;
            اگر (asdl_seq_LEN(مقادیر) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                بازگشت پوچ;
            }

            عبارت_ty first = asdl_seq_GET(مقادیر, 0);
            أكد(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            عبارت_ty second = asdl_seq_GET(مقادیر, 1);
            أكد((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_مقدار_kind);
            asdl_seq_SET(seq, index++, second);

            ادامه;
        }

        اگر (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            اگر (item == پوچ) {
                بازگشت پوچ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            اگر (PyUnicode_CheckExact(item->v.Constant.مقدار)
                && PyUnicode_GET_LENGTH(item->v.Constant.مقدار) == 0) {
                ادامه;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_عبارت_seq *resized_exprs;
    اگر (index != total_اقلام) {
        resized_exprs = Py_asdl_عبارت_seq_new(index, p->arena);
        اگر (resized_exprs == پوچ) {
            بازگشت پوچ;
        }
        برای (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وگرنه {
        resized_exprs = seq;
    }
    بازگشت resized_exprs;
}

عبارت_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b) {

    asdl_عبارت_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    اگر (resized_exprs == پوچ) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

عبارت_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_عبارت_seq* raw_expressions, Token*b) {

    asdl_عبارت_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    اگر (resized_exprs == پوچ) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

عبارت_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    اگر (tok->bytes == پوچ) {
        بازگشت Py_Pegen_raise_incomplete_درون_داد(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ثابت نویسه *bstr = PyBytes_AS_STRING(tok->bytes);

    صحیح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    اگر (str == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        بازگشت پوچ;
    }
    بازگشت _PyAST_Constant(str, پوچ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

عبارت_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    اگر (tok->bytes == پوچ) {
        بازگشت Py_Pegen_raise_incomplete_درون_داد(p);
    }
    ثابت نویسه *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    اگر (str == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        بازگشت پوچ;
    }
    بازگشت _PyAST_Constant(str, پوچ, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

عبارت_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    اگر (tok->bytes == پوچ) {
        بازگشت Py_Pegen_raise_incomplete_درون_داد(p);
    }
    ثابت نویسه *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    اگر (s == پوچ) {
        _Pypegen_raise_decode_error(p);
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        بازگشت پوچ;
    }
    PyObject *kind = پوچ;
    اگر (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        اگر (kind == پوچ) {
            بازگشت پوچ;
        }
    }
    بازگشت _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ایستا صحیح
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    اگر (conversion != پوچ) {
        عبارت_ty conversion_عبارت = (عبارت_ty) conversion->result;
        أكد(conversion_عبارت->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_عبارت->v.Name.id, 0);
        بازگشت Py_SAFE_DOWNCAST(first, Py_UCS4, صحیح);
    }
    وگرنه اگر (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        بازگشت (صحیح)'r';
    }
    بازگشت -1;
}

ایستا PyObject *
strip_interpolation_debug_عبارت(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    وقتی (len > 0) {
        صحیح has_newline = 0;
        وقتی (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            اگر (!_PyUnicode_IsWhitespace(c)) {
                بشکن;
            }
            اگر (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        اگر (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            بشکن;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    اگر (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        بازگشت Py_NewRef(exprstr);
    }

    بازگشت PyUnicode_Substring(exprstr, 0, len - 1);
}

عبارت_ty _PyPegen_interpolation(Parser *p, عبارت_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحیح lineno, صحیح col_offset,
                                 صحیح end_lineno, صحیح end_col_offset, PyArena *arena) {

    صحیح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحیح debug_end_سطر, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    اگر (conversion) {
        debug_end_سطر = ((عبارت_ty) conversion->result)->lineno;
        debug_end_offset = ((عبارت_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    وگرنه اگر (format) {
        debug_end_سطر = ((عبارت_ty) format->result)->lineno;
        debug_end_offset = ((عبارت_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وگرنه {
        debug_end_سطر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != پوچ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_عبارت(exprstr)
        : Py_NewRef(exprstr);
    اگر (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        بازگشت پوچ;
    }

    عبارت_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (عبارت_ty) format->result : پوچ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    اگر (!interpolation || !debug) {
        بازگشت interpolation;
    }

    عبارت_ty debug_text = _PyAST_Constant(debug_metadata, پوچ, lineno, col_offset + 1, debug_end_سطر,
                                            debug_end_offset - 1, p->arena);
    اگر (!debug_text) {
        بازگشت پوچ;
    }

    asdl_عبارت_seq *مقادیر = Py_asdl_عبارت_seq_new(2, arena);
    اگر (!مقادیر) {
        بازگشت پوچ;
    }
    asdl_seq_SET(مقادیر, 0, debug_text);
    asdl_seq_SET(مقادیر, 1, interpolation);
    بازگشت _PyAST_JoinedStr(مقادیر, lineno, col_offset, debug_end_سطر, debug_end_offset, p->arena);
}

عبارت_ty Py_Pegen_formatted_مقدار(Parser *p, عبارت_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحیح lineno, صحیح col_offset,
                                 صحیح end_lineno, صحیح end_col_offset, PyArena *arena) {
    صحیح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    عبارت_ty formatted_مقدار = Py_AST_Formatted_مقدار(
        expression, conversion_val, format ? (عبارت_ty) format->result : پوچ,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    اگر (!formatted_مقدار || !debug) {
        بازگشت formatted_مقدار;
    }

    /* Find the non whitespace token after the "=" */
    صحیح debug_end_سطر, debug_end_offset;
    PyObject *debug_metadata;

    اگر (conversion) {
        debug_end_سطر = ((عبارت_ty) conversion->result)->lineno;
        debug_end_offset = ((عبارت_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    وگرنه اگر (format) {
        debug_end_سطر = ((عبارت_ty) format->result)->lineno;
        debug_end_offset = ((عبارت_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    وگرنه {
        debug_end_سطر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    عبارت_ty debug_text = _PyAST_Constant(debug_metadata, پوچ, lineno, col_offset + 1, debug_end_سطر,
                                            debug_end_offset - 1, p->arena);
    اگر (!debug_text) {
        بازگشت پوچ;
    }

    asdl_عبارت_seq *مقادیر = Py_asdl_عبارت_seq_new(2, arena);
    اگر (!مقادیر) {
        بازگشت پوچ;
    }
    asdl_seq_SET(مقادیر, 0, debug_text);
    asdl_seq_SET(مقادیر, 1, formatted_مقدار);
    بازگشت _PyAST_JoinedStr(مقادیر, lineno, col_offset, debug_end_سطر, debug_end_offset, p->arena);
}

ایستا عبارت_ty
_build_concatenated_bytes(Parser *p, asdl_عبارت_seq *strings, صحیح lineno,
                        صحیح col_offset, صحیح end_lineno, صحیح end_col_offset,
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
    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.مقدار;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        اگر (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            بازگشت پوچ;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    اگر (writer == پوچ) {
        بازگشت پوچ;
    }
    نویسه *out = PyBytesWriter_GetData(writer);

    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.مقدار;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        اگر (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    اگر (res == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        بازگشت پوچ;
    }
    بازگشت _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ایستا عبارت_ty
_build_concatenated_unicode(Parser *p, asdl_عبارت_seq *strings, صحیح lineno,
                        صحیح col_offset, صحیح end_lineno, صحیح end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 1);

    عبارت_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    اگر (writer == پوچ) {
        بازگشت پوچ;
    }

    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty current_elem = asdl_seq_GET(strings, i);
        أكد(current_elem->kind == Constant_kind);

        اگر (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.مقدار)) {
            PyUnicodeWriter_Discard(writer);
            بازگشت پوچ;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    اگر (final == پوچ) {
        بازگشت پوچ;
    }
    اگر (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        بازگشت پوچ;
    }
    بازگشت _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ایستا asdl_عبارت_seq *
_build_concatenated_str(Parser *p, asdl_عبارت_seq *strings,
                               صحیح lineno, صحیح col_offset, صحیح end_lineno,
                               صحیح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(strings, i);
        انتخاب(elem->kind) {
            حالت JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.مقادیر);
                بشکن;
            حالت TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.مقادیر);
                بشکن;
            پیشفرض:
                n_flattened_elements++;
                بشکن;
        }
    }


    asdl_عبارت_seq* flattened = Py_asdl_عبارت_seq_new(n_flattened_elements, p->arena);
    اگر (flattened == پوچ) {
        بازگشت پوچ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    برای (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(strings, i);
        انتخاب(elem->kind) {
            حالت JoinedStr_kind:
                برای (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.مقادیر); j++) {
                    عبارت_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.مقادیر, j);
                    اگر (subvalue == پوچ) {
                        بازگشت پوچ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                بشکن;
            حالت TemplateStr_kind:
                برای (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.مقادیر); j++) {
                    عبارت_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.مقادیر, j);
                    اگر (subvalue == پوچ) {
                        بازگشت پوچ;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                بشکن;
            پیشفرض:
                asdl_seq_SET(flattened, current_pos++, elem);
                بشکن;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحیح prev_is_constant = 0;
    برای (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        عبارت_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        اگر (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.مقدار) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.مقدار) == 0)
            ادامه;

        اگر (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_عبارت_seq* مقادیر = Py_asdl_عبارت_seq_new(n_elements, p->arena);
    اگر (مقادیر == پوچ) {
        بازگشت پوچ;
    }

    /* build folded list */
    current_pos = 0;
    برای (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        عبارت_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        اگر (elem->kind == Constant_kind) {
            اگر (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                عبارت_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                اگر (writer == پوچ) {
                    بازگشت پوچ;
                }
                عبارت_ty last_elem = elem;
                Py_ssize_t j;
                برای (j = i; j < n_flattened_elements; j++) {
                    عبارت_ty current_elem = asdl_seq_GET(flattened, j);
                    اگر (current_elem->kind == Constant_kind) {
                        اگر (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.مقدار)) {
                            PyUnicodeWriter_Discard(writer);
                            بازگشت پوچ;
                        }
                        last_elem = current_elem;
                    } وگرنه {
                        بشکن;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                اگر (concat_str == پوچ) {
                    بازگشت پوچ;
                }
                اگر (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    بازگشت پوچ;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                اگر (elem == پوچ) {
                    بازگشت پوچ;
                }
            }

            /* Drop all empty contanst strings */
            اگر (PyUnicode_CheckExact(elem->v.Constant.مقدار) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.مقدار) == 0) {
                ادامه;
            }
        }

        asdl_seq_SET(مقادیر, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    بازگشت مقادیر;
}

ایستا عبارت_ty
_build_concatenated_joined_str(Parser *p, asdl_عبارت_seq *strings,
                               صحیح lineno, صحیح col_offset, صحیح end_lineno,
                               صحیح end_col_offset, PyArena *arena)
{
    asdl_عبارت_seq *مقادیر = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    اگر (!مقادیر) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_JoinedStr(مقادیر, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

عبارت_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_عبارت_seq *strings,
                               صحیح lineno, صحیح col_offset, صحیح end_lineno,
                               صحیح end_col_offset, PyArena *arena)
{
    asdl_عبارت_seq *مقادیر = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    اگر (!مقادیر) {
        بازگشت پوچ;
    }
    بازگشت _PyAST_TemplateStr(مقادیر, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

عبارت_ty
_PyPegen_concatenate_strings(Parser *p, asdl_عبارت_seq *strings,
                             صحیح lineno, صحیح col_offset, صحیح end_lineno,
                             صحیح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    صحیح f_string_found = 0;
    صحیح unicode_string_found = 0;
    صحیح bytes_found = 0;

    Py_ssize_t i = 0;
    برای (i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(strings, i);
        انتخاب(elem->kind) {
            حالت Constant_kind:
                اگر (PyBytes_CheckExact(elem->v.Constant.مقدار)) {
                    bytes_found = 1;
                } وگرنه {
                    unicode_string_found = 1;
                }
                بشکن;
            حالت JoinedStr_kind:
                f_string_found = 1;
                بشکن;
            حالت TemplateStr_kind:
                // python.gram handles this; we should never get here
                أكد(0);
                بشکن;
            پیشفرض:
                f_string_found = 1;
                بشکن;
        }
    }

    // Cannot mix unicode and bytes
    اگر ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        بازگشت پوچ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    اگر (!f_string_found) {
        اگر (len == 1) {
            بازگشت asdl_seq_GET(strings, 0);
        }
        وگرنه اگر (bytes_found) {
            بازگشت _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        وگرنه {
            بازگشت _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    بازگشت _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ایستا صحیح
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, عبارت_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    اگر (p->call_invalid_rules) {
        بازگشت 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    اگر (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        بازگشت 0;
    }

    صحیح شمار = Py_Pegen_seq_شمار_dots(dots);
    نویسه *buf = PyMem_RawMalloc(شمار + 1);
    اگر (buf == پوچ) {
        PyErr_NoMemory();
        بازگشت -1;
    }
    memset(buf, '.', شمار);
    buf[شمار] = '\0';

    PyObject *پیام = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    اگر (پیام == پوچ) {
        بازگشت -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحیح res = _PyErr_EmitSyntaxWarning(پیام,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(پیام);
    بازگشت res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, عبارت_ty module_name,
                             asdl_alias_seq *names, عبارت_ty lazy_token, صحیح lineno,
                             صحیح col_offset, صحیح end_lineno, صحیح end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحیح level = Py_Pegen_seq_شمار_dots(dots);
    اگر (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        اگر (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            بازگشت پوچ;
        }
        برای (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            اگر (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    وگرنه اگر (
        level > 0
        && lazy_token == پوچ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        اگر (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            بازگشت پوچ;
        }
    }
    بازگشت _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    اگر (!p->call_invalid_rules) {
        بازگشت stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    اگر (len == 0) {
        بازگشت stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    اگر (p->last_stmt_location.lineno > last_stmt->lineno) {
        بازگشت stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    بازگشت stmts;
}
