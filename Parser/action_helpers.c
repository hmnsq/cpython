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
    إرجاع &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, فراغ *a)
{
    أكد(a != عدم);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    إذا (!seq) {
        إرجاع عدم;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    إرجاع seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_أدرج_in_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    إذا (!seq) {
        إرجاع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    إرجاع new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, فراغ *a)
{
    أكد(a != عدم);
    إذا (!seq) {
        إرجاع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }

    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    إرجاع new_seq;
}

ساكن Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    إرجاع size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    أكد(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    إذا (!flattened_seq) {
        إرجاع عدم;
    }

    صحيح flattened_seq_idx = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        لكل (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_size);

    إرجاع flattened_seq;
}

فراغ *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إرجاع asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    إرجاع asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
تعبير_ty
_PyPegen_join_names_with_dot(Parser *p, تعبير_ty first_name, تعبير_ty second_name)
{
    أكد(first_name != عدم && second_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    إذا (!uni) {
        إرجاع عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    إذا (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        إرجاع عدم;
    }

    إرجاع _PyAST_Name(uni, Load, EXTRA_تعبير(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
Py_Pegen_seq_تعداد_dots(asdl_seq *seq)
{
    صحيح العدد_of_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_تعبير = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_تعبير->type) {
            حالة ELLIPSIS:
                العدد_of_dots += 3;
                اكسر;
            حالة DOT:
                العدد_of_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    إرجاع العدد_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, صحيح lineno, صحيح col_offset, صحيح end_lineno,
                        صحيح end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    إذا (!str) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        إرجاع عدم;
    }
    إرجاع _PyAST_alias(str, عدم, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_تعبير_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    إرجاع new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_تعبير_Pair *
Py_Pegen_cmpop_تعبير_pair(Parser *p, cmpop_ty cmpop, تعبير_ty تعبير)
{
    أكد(تعبير != عدم);
    Cmpop_تعبير_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_تعبير_Pair));
    إذا (!a) {
        إرجاع عدم;
    }
    a->cmpop = cmpop;
    a->تعبير = تعبير;
    إرجاع a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_تعبير_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    إرجاع new_seq;
}

asdl_تعبير_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_تعبير_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->تعبير);
    }
    إرجاع new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_تعبير_seq *
_set_seq_context(Parser *p, asdl_تعبير_seq *seq, تعبير_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إذا (len == 0) {
        إرجاع عدم;
    }

    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty e = asdl_seq_GET(seq, i);
        تعبير_ty new_e = Py_Pegen_set_تعبير_context(p, e, ctx);
        إذا (!new_e) {
            إرجاع عدم;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    إرجاع new_seq;
}

ساكن تعبير_ty
_set_name_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    إرجاع _PyAST_Name(e->v.Name.id, ctx, EXTRA_تعبير(e, e));
}

ساكن تعبير_ty
_set_tuple_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    asdl_تعبير_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        إرجاع عدم;
    }
    إرجاع _PyAST_Tuple(seq, ctx, EXTRA_تعبير(e, e));
}

ساكن تعبير_ty
_set_list_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    asdl_تعبير_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        إرجاع عدم;
    }
    إرجاع _PyAST_List(seq, ctx, EXTRA_تعبير(e, e));
}

ساكن تعبير_ty
_set_subscript_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    إرجاع _PyAST_Subscript(e->v.Subscript.قيمة, e->v.Subscript.slice,
                            ctx, EXTRA_تعبير(e, e));
}

ساكن تعبير_ty
_set_attribute_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    إرجاع _PyAST_Attribute(e->v.Attribute.قيمة, e->v.Attribute.attr,
                            ctx, EXTRA_تعبير(e, e));
}

ساكن تعبير_ty
_set_starred_context(Parser *p, تعبير_ty e, تعبير_context_ty ctx)
{
    تعبير_ty inner = Py_Pegen_set_تعبير_context(p, e->v.Starred.قيمة, ctx);
    إذا (!inner) {
        إرجاع عدم;
    }
    إرجاع _PyAST_Starred(inner, ctx, EXTRA_تعبير(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
تعبير_ty
Py_Pegen_set_تعبير_context(Parser *p, تعبير_ty تعبير, تعبير_context_ty ctx)
{
    أكد(تعبير != عدم);

    تعبير_ty new = عدم;
    بدل (تعبير->kind) {
        حالة Name_kind:
            new = _set_name_context(p, تعبير, ctx);
            اكسر;
        حالة Tuple_kind:
            new = _set_tuple_context(p, تعبير, ctx);
            اكسر;
        حالة List_kind:
            new = _set_list_context(p, تعبير, ctx);
            اكسر;
        حالة Subscript_kind:
            new = _set_subscript_context(p, تعبير, ctx);
            اكسر;
        حالة Attribute_kind:
            new = _set_attribute_context(p, تعبير, ctx);
            اكسر;
        حالة Starred_kind:
            new = _set_starred_context(p, تعبير, ctx);
            اكسر;
        افتراضي:
            new = تعبير;
    }
    إرجاع new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
تعبير_ty
_PyPegen_make_target(Parser *p, تعبير_ty تعبير, TARGETS_TYPE targets_type)
{
    أكد(تعبير != عدم);
    أكد(targets_type != FOR_TARGETS);
    إذا (_PyPegen_get_invalid_target(تعبير, targets_type) != عدم) {
        إرجاع عدم;
    }
    إرجاع Py_Pegen_set_تعبير_context(
        p, تعبير, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
مفتاح_قيمة_Pair *
Py_Pegen_مفتاح_قيمة_pair(Parser *p, تعبير_ty مفتاح, تعبير_ty قيمة)
{
    مفتاح_قيمة_Pair *a = _PyArena_Malloc(p->arena, حجم(مفتاح_قيمة_Pair));
    إذا (!a) {
        إرجاع عدم;
    }
    a->مفتاح = مفتاح;
    a->قيمة = قيمة;
    إرجاع a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_تعبير_seq *
Py_Pegen_get_مفاتيح(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        مفتاح_قيمة_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->مفتاح);
    }
    إرجاع new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_تعبير_seq *
Py_Pegen_get_قيم(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        مفتاح_قيمة_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->قيمة);
    }
    إرجاع new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
مفتاح_Pattern_Pair *
Py_Pegen_مفتاح_pattern_pair(Parser *p, تعبير_ty مفتاح, pattern_ty pattern)
{
    مفتاح_Pattern_Pair *a = _PyArena_Malloc(p->arena, حجم(مفتاح_Pattern_Pair));
    إذا (!a) {
        إرجاع عدم;
    }
    a->مفتاح = مفتاح;
    a->pattern = pattern;
    إرجاع a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_تعبير_seq *
Py_Pegen_get_pattern_مفاتيح(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        مفتاح_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->مفتاح);
    }
    إرجاع new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        مفتاح_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    إرجاع new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, تعبير_ty قيمة, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    إذا (!a) {
        إرجاع عدم;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    إذا (!a->arg) {
        إرجاع عدم;
    }
    a->قيمة = قيمة;
    إرجاع a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    إذا (!a) {
        إرجاع عدم;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    إرجاع a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, حجم(StarEtc));
    إذا (!a) {
        إرجاع عدم;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    إرجاع a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    إرجاع new_seq;
}

ساكن asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    إذا (!seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    إرجاع seq;
}

ساكن asdl_تعبير_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_تعبير_seq *seq = Py_asdl_تعبير_seq_new(len, p->arena);
    إذا (!seq) {
        إرجاع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->قيمة);
    }
    إرجاع seq;
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
            إرجاع -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    وإلا {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    إرجاع *posonlyargs == عدم ? -1 : 0;
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
                إرجاع -1;
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
    إرجاع *posargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_تعبير_seq **posdefaults) {
    إذا (slash_with_default != عدم && names_with_default != عدم) {
        asdl_تعبير_seq *slash_with_default_قيم =
                _get_defaults(p, slash_with_default->names_with_defaults);
        إذا (!slash_with_default_قيم) {
            إرجاع -1;
        }
        asdl_تعبير_seq *names_with_default_قيم = _get_defaults(p, names_with_default);
        إذا (!names_with_default_قيم) {
            إرجاع -1;
        }
        *posdefaults = (asdl_تعبير_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_قيم,
                (asdl_seq*)names_with_default_قيم);
    }
    وإلا إذا (slash_with_default == عدم && names_with_default != عدم) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    وإلا إذا (slash_with_default != عدم && names_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    وإلا {
        *posdefaults = Py_asdl_تعبير_seq_new(0, p->arena);
    }
    إرجاع *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_تعبير_seq **kwdefaults) {
    إذا (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    وإلا {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    إذا (*kwonlyargs == عدم) {
        إرجاع -1;
    }

    إذا (star_etc != عدم && star_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    وإلا {
        *kwdefaults = Py_asdl_تعبير_seq_new(0, p->arena);
    }

    إذا (*kwdefaults == عدم) {
        إرجاع -1;
    }

    إرجاع 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    إذا (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        إرجاع عدم;
    }

    asdl_arg_seq *posargs;
    إذا (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        إرجاع عدم;
    }

    asdl_تعبير_seq *posdefaults;
    إذا (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        إرجاع عدم;
    }

    arg_ty vararg = عدم;
    إذا (star_etc != عدم && star_etc->vararg != عدم) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_تعبير_seq *kwdefaults;
    إذا (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        إرجاع عدم;
    }

    arg_ty kwarg = عدم;
    إذا (star_etc != عدم && star_etc->kwarg != عدم) {
        kwarg = star_etc->kwarg;
    }

    إرجاع _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posonlyargs) {
        إرجاع عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posargs) {
        إرجاع عدم;
    }
    asdl_تعبير_seq *posdefaults = Py_asdl_تعبير_seq_new(0, p->arena);
    إذا (!posdefaults) {
        إرجاع عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!kwonlyargs) {
        إرجاع عدم;
    }
    asdl_تعبير_seq *kwdefaults = Py_asdl_تعبير_seq_new(0, p->arena);
    إذا (!kwdefaults) {
        إرجاع عدم;
    }

    إرجاع _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    إذا (!a) {
        إرجاع عدم;
    }
    a->kind = kind;
    إرجاع a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_تعبير_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    إذا (function_def->kind == AsyncFunctionDef_kind) {
        إرجاع _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    إرجاع _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_تعبير_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != عدم);
    إرجاع _PyAST_ClassDef(
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
        إرجاع عدم;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    إرجاع a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ساكن صحيح
seq_العدد_of_starred_exprs(asdl_seq *seq)
{
    صحيح n = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        إذا (!k->is_keyword) {
            n++;
        }
    }
    إرجاع n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_تعبير_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = seq_العدد_of_starred_exprs(kwargs);
    إذا (new_len == 0) {
        إرجاع عدم;
    }
    asdl_تعبير_seq *new_seq = Py_asdl_تعبير_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    إرجاع new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_العدد_of_starred_exprs(kwargs);
    إذا (new_len == 0) {
        إرجاع عدم;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        إرجاع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    إرجاع new_seq;
}

تعبير_ty
_PyPegen_ensure_imaginary(Parser *p, تعبير_ty exp)
{
    إذا (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.قيمة)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        إرجاع عدم;
    }
    إرجاع exp;
}

تعبير_ty
_PyPegen_ensure_real(Parser *p, تعبير_ty exp)
{
    إذا (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.قيمة)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        إرجاع عدم;
    }
    إرجاع exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t رقم = p->type_ignore_comments.رقم_عناصر;
    إذا (رقم > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(رقم, p->arena);
        إذا (type_ignores == عدم) {
            إرجاع عدم;
        }
        لكل (Py_ssize_t i = 0; i < رقم; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.عناصر[i].comment);
            إذا (tag == عدم) {
                إرجاع عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.عناصر[i].lineno,
                                                  tag, p->arena);
            إذا (ti == عدم) {
                إرجاع عدم;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    إرجاع _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), عدم);
    إذا (res == عدم) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        إرجاع عدم;
    }
    إرجاع res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    إذا (tc == عدم) {
        إرجاع a;
    }
    ثابت حرف *bytes = PyBytes_AsString(tc->bytes);
    إذا (bytes == عدم) {
        إرجاع عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    إذا (tco == عدم) {
        إرجاع عدم;
    }
    إرجاع _PyAST_arg(a->arg, a->annotation, tco,
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
        إرجاع -1;
    }
    إذا (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        إرجاع strcmp(tok_str, "!=");
    }
    إرجاع 0;
}

صحيح
_PyPegen_check_legacy_stmt(Parser *p, تعبير_ty name) {
    إذا (name->kind != Name_kind) {
        إرجاع 0;
    }
    ثابت حرف* candidates[2] = {"print", "exec"};
    لكل (صحيح i=0; i<2; i++) {
        إذا (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            إرجاع 1;
        }
    }
    إرجاع 0;
}

فراغ *
_PyPegen_raise_error_for_missing_comma(Parser *p, تعبير_ty a, تعبير_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    إذا (_PyPegen_check_legacy_stmt(p, a)) {
        إرجاع عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    إذا (p->tokens[p->mark - 1]->level == 0) {
        إرجاع عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    إذا (a->end_lineno > a->lineno) {
        إرجاع RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    إرجاع RAISE_ERROR_KNOWN_LOCATION(
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
        إرجاع عدم;
    }
    res->metadata = metadata;
    res->result = result;
    إرجاع res;
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
                إرجاع type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, تعبير_ty conv)
{
    إذا (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        إرجاع RAISE_SYNTAX_ERROR_KNOWN_RANGE(
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
        إرجاع عدم;
    }

    إرجاع result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_تعبير_seq *spec, صحيح lineno, صحيح col_offset,
                                صحيح end_lineno, صحيح end_col_offset, PyArena *arena)
{
    إذا (!spec) {
        إرجاع عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_عناصر = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_تعداد = 0;
    لكل (Py_ssize_t i = 0; i < n_عناصر; i++) {
        تعبير_ty item = asdl_seq_GET(spec, i);
        non_empty_تعداد += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.قيمة) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.قيمة) == 0);
    }
    إذا (non_empty_تعداد != n_عناصر) {
        asdl_تعبير_seq *resized_spec =
            Py_asdl_تعبير_seq_new(non_empty_تعداد, p->arena);
        إذا (resized_spec == عدم) {
            إرجاع عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_عناصر; i++) {
            تعبير_ty item = asdl_seq_GET(spec, i);
            إذا (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.قيمة) &&
                PyUnicode_GET_LENGTH(item->v.Constant.قيمة) == 0) {
                استمر;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        أكد(j == non_empty_تعداد);
        spec = resized_spec;
    }
    تعبير_ty res;
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
        إرجاع عدم;
    }
    إرجاع result_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_تعبير_name(تعبير_ty e)
{
    أكد(e != عدم);
    بدل (e->kind) {
        حالة Attribute_kind:
            إرجاع "attribute";
        حالة Subscript_kind:
            إرجاع "subscript";
        حالة Starred_kind:
            إرجاع "starred";
        حالة Name_kind:
            إرجاع "name";
        حالة List_kind:
            إرجاع "list";
        حالة Tuple_kind:
            إرجاع "tuple";
        حالة Lambda_kind:
            إرجاع "lambda";
        حالة Call_kind:
            إرجاع "function call";
        حالة BoolOp_kind:
        حالة BinOp_kind:
        حالة UnaryOp_kind:
            إرجاع "expression";
        حالة GeneratorExp_kind:
            إرجاع "generator expression";
        حالة Yield_kind:
        حالة YieldFrom_kind:
            إرجاع "yield expression";
        حالة Await_kind:
            إرجاع "await expression";
        حالة ListComp_kind:
            إرجاع "list comprehension";
        حالة SetComp_kind:
            إرجاع "set comprehension";
        حالة DictComp_kind:
            إرجاع "dict comprehension";
        حالة Dict_kind:
            إرجاع "dict literal";
        حالة Set_kind:
            إرجاع "set display";
        حالة JoinedStr_kind:
        حالة Formatted_قيمة_kind:
            إرجاع "f-string expression";
        حالة TemplateStr_kind:
        حالة Interpolation_kind:
            إرجاع "t-string expression";
        حالة Constant_kind: {
            PyObject *قيمة = e->v.Constant.قيمة;
            إذا (قيمة == Py_None) {
                إرجاع "None";
            }
            إذا (قيمة == Py_False) {
                إرجاع "False";
            }
            إذا (قيمة == Py_True) {
                إرجاع "True";
            }
            إذا (قيمة == Py_Ellipsis) {
                إرجاع "ellipsis";
            }
            إرجاع "literal";
        }
        حالة Compare_kind:
            إرجاع "comparison";
        حالة IfExp_kind:
            إرجاع "conditional expression";
        حالة Named_تعبير_kind:
            إرجاع "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            إرجاع عدم;
    }
}

تعبير_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    إذا (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        إرجاع comprehension->iter;
    }
    إرجاع PyPegen_last_item(comprehension->ifs, تعبير_ty);
}

تعبير_ty _PyPegen_collect_call_seqs(Parser *p, asdl_تعبير_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح col_offset, صحيح end_lineno,
                     صحيح end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    إذا (b == عدم) {
        إرجاع _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_تعبير_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    إذا (!starreds && PyErr_Occurred()) {
        إرجاع عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    إذا (!keywords && PyErr_Occurred()) {
        إرجاع عدم;
    }

    إذا (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_تعبير_seq *args = Py_asdl_تعبير_seq_new(total_len, arena);
    إذا (args == عدم) {
        إرجاع عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    لكل (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    إرجاع _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

تعبير_ty
_PyPegen_get_invalid_target(تعبير_ty e, TARGETS_TYPE targets_type)
{
    إذا (e == عدم) {
        إرجاع عدم;
    }

    إذا (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        إذا (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            إرجاع عدم;
        }
        إرجاع e->kind == Attribute_kind || e->kind == Subscript_kind ? عدم : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) افعل { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            تعبير_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            تعبير_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            إذا (child != عدم) {\
                إرجاع child;\
            }\
        }\
    } بينما (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->kind) {
        حالة List_kind:
            VISIT_CONTAINER(e, List);
            إرجاع عدم;
        حالة Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            إرجاع عدم;
        حالة Starred_kind:
            إذا (targets_type == DEL_TARGETS) {
                إرجاع e;
            }
            إرجاع _PyPegen_get_invalid_target(e->v.Starred.قيمة, targets_type);
        حالة Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            إذا (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                إذا (cmpop == In) {
                    إرجاع _PyPegen_get_invalid_target(e->v.Compare.يسار, targets_type);
                }
                إرجاع عدم;
            }
            إرجاع e;
        حالة Name_kind:
        حالة Subscript_kind:
        حالة Attribute_kind:
            إرجاع عدم;
        افتراضي:
            إرجاع e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, تعبير_ty e) {
    صحيح kwarg_unpacking = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        إذا (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *رسالة = عدم;
    إذا (kwarg_unpacking) {
        رسالة = "positional argument follows keyword argument unpacking";
    } وإلا {
        رسالة = "positional argument follows keyword argument";
    }

    إرجاع RAISE_SYNTAX_ERROR(رسالة);
}

فراغ *
_PyPegen_nonparen_genexp_in_call(Parser *p, تعبير_ty args, asdl_comprehension_seq *comprehensions)
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
        إرجاع عدم;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    إرجاع RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (تعبير_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن تعبير_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, تعبير_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.قيمة));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.قيمة);
    إذا (bstr == عدم) {
        إرجاع عدم;
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
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        إرجاع عدم;
    }
    إرجاع _PyAST_Constant(str, عدم, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

ساكن asdl_تعبير_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_تعبير_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_عناصر = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_عناصر = n_عناصر;
    لكل (Py_ssize_t i = 0; i < n_عناصر; i++) {
        تعبير_ty item = asdl_seq_GET(raw_expressions, i);
        إذا (item->kind == JoinedStr_kind) {
            total_عناصر += asdl_seq_LEN(item->v.JoinedStr.قيم) - 1;
        }
    }

    ثابت حرف* quote_str = PyBytes_AsString(a->bytes);
    إذا (quote_str == عدم) {
        إرجاع عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_تعبير_seq *seq = Py_asdl_تعبير_seq_new(total_عناصر, p->arena);
    إذا (seq == عدم) {
        إرجاع عدم;
    }

    Py_ssize_t index = 0;
    لكل (Py_ssize_t i = 0; i < n_عناصر; i++) {
        تعبير_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        إذا (item->kind == JoinedStr_kind) {
            asdl_تعبير_seq *قيم = item->v.JoinedStr.قيم;
            إذا (asdl_seq_LEN(قيم) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                إرجاع عدم;
            }

            تعبير_ty first = asdl_seq_GET(قيم, 0);
            أكد(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            تعبير_ty second = asdl_seq_GET(قيم, 1);
            أكد((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_قيمة_kind);
            asdl_seq_SET(seq, index++, second);

            استمر;
        }

        إذا (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            إذا (item == عدم) {
                إرجاع عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            إذا (PyUnicode_CheckExact(item->v.Constant.قيمة)
                && PyUnicode_GET_LENGTH(item->v.Constant.قيمة) == 0) {
                استمر;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_تعبير_seq *resized_exprs;
    إذا (index != total_عناصر) {
        resized_exprs = Py_asdl_تعبير_seq_new(index, p->arena);
        إذا (resized_exprs == عدم) {
            إرجاع عدم;
        }
        لكل (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وإلا {
        resized_exprs = seq;
    }
    إرجاع resized_exprs;
}

تعبير_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_تعبير_seq *raw_expressions, Token *b) {

    asdl_تعبير_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    إذا (resized_exprs == عدم) {
        إرجاع عدم;
    }
    إرجاع _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

تعبير_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_تعبير_seq* raw_expressions, Token*b) {

    asdl_تعبير_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    إذا (resized_exprs == عدم) {
        إرجاع عدم;
    }
    إرجاع _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

تعبير_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        إرجاع Py_Pegen_raise_incomplete_إدخال(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    إذا (str == عدم) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        إرجاع عدم;
    }
    إرجاع _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

تعبير_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        إرجاع Py_Pegen_raise_incomplete_إدخال(p);
    }
    ثابت حرف *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    إذا (str == عدم) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        إرجاع عدم;
    }
    إرجاع _PyAST_Constant(str, عدم, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

تعبير_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    إذا (tok->bytes == عدم) {
        إرجاع Py_Pegen_raise_incomplete_إدخال(p);
    }
    ثابت حرف *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    إذا (s == عدم) {
        _Pypegen_raise_decode_error(p);
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        إرجاع عدم;
    }
    PyObject *kind = عدم;
    إذا (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        إذا (kind == عدم) {
            إرجاع عدم;
        }
    }
    إرجاع _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    إذا (conversion != عدم) {
        تعبير_ty conversion_تعبير = (تعبير_ty) conversion->result;
        أكد(conversion_تعبير->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_تعبير->v.Name.id, 0);
        إرجاع Py_SAFE_DOWNCAST(first, Py_UCS4, صحيح);
    }
    وإلا إذا (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        إرجاع (صحيح)'r';
    }
    إرجاع -1;
}

ساكن PyObject *
strip_interpolation_debug_تعبير(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    بينما (len > 0) {
        صحيح has_newline = 0;
        بينما (len > 0) {
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
        إرجاع Py_NewRef(exprstr);
    }

    إرجاع PyUnicode_Substring(exprstr, 0, len - 1);
}

تعبير_ty _PyPegen_interpolation(Parser *p, تعبير_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_سطر, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    إذا (conversion) {
        debug_end_سطر = ((تعبير_ty) conversion->result)->lineno;
        debug_end_offset = ((تعبير_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_end_سطر = ((تعبير_ty) format->result)->lineno;
        debug_end_offset = ((تعبير_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وإلا {
        debug_end_سطر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_تعبير(exprstr)
        : Py_NewRef(exprstr);
    إذا (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        إرجاع عدم;
    }

    تعبير_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (تعبير_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    إذا (!interpolation || !debug) {
        إرجاع interpolation;
    }

    تعبير_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_سطر,
                                            debug_end_offset - 1, p->arena);
    إذا (!debug_text) {
        إرجاع عدم;
    }

    asdl_تعبير_seq *قيم = Py_asdl_تعبير_seq_new(2, arena);
    إذا (!قيم) {
        إرجاع عدم;
    }
    asdl_seq_SET(قيم, 0, debug_text);
    asdl_seq_SET(قيم, 1, interpolation);
    إرجاع _PyAST_JoinedStr(قيم, lineno, col_offset, debug_end_سطر, debug_end_offset, p->arena);
}

تعبير_ty Py_Pegen_formatted_قيمة(Parser *p, تعبير_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, صحيح lineno, صحيح col_offset,
                                 صحيح end_lineno, صحيح end_col_offset, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    تعبير_ty formatted_قيمة = Py_AST_Formatted_قيمة(
        expression, conversion_val, format ? (تعبير_ty) format->result : عدم,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    إذا (!formatted_قيمة || !debug) {
        إرجاع formatted_قيمة;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_end_سطر, debug_end_offset;
    PyObject *debug_metadata;

    إذا (conversion) {
        debug_end_سطر = ((تعبير_ty) conversion->result)->lineno;
        debug_end_offset = ((تعبير_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_end_سطر = ((تعبير_ty) format->result)->lineno;
        debug_end_offset = ((تعبير_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    وإلا {
        debug_end_سطر = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    تعبير_ty debug_text = _PyAST_Constant(debug_metadata, عدم, lineno, col_offset + 1, debug_end_سطر,
                                            debug_end_offset - 1, p->arena);
    إذا (!debug_text) {
        إرجاع عدم;
    }

    asdl_تعبير_seq *قيم = Py_asdl_تعبير_seq_new(2, arena);
    إذا (!قيم) {
        إرجاع عدم;
    }
    asdl_seq_SET(قيم, 0, debug_text);
    asdl_seq_SET(قيم, 1, formatted_قيمة);
    إرجاع _PyAST_JoinedStr(قيم, lineno, col_offset, debug_end_سطر, debug_end_offset, p->arena);
}

ساكن تعبير_ty
_build_concatenated_bytes(Parser *p, asdl_تعبير_seq *strings, صحيح lineno,
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
        تعبير_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.قيمة;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        إذا (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            إرجاع عدم;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    إذا (writer == عدم) {
        إرجاع عدم;
    }
    حرف *out = PyBytesWriter_GetData(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.قيمة;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        إذا (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    إذا (res == عدم) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        إرجاع عدم;
    }
    إرجاع _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

ساكن تعبير_ty
_build_concatenated_unicode(Parser *p, asdl_تعبير_seq *strings, صحيح lineno,
                        صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 1);

    تعبير_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    إذا (writer == عدم) {
        إرجاع عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty current_elem = asdl_seq_GET(strings, i);
        أكد(current_elem->kind == Constant_kind);

        إذا (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.قيمة)) {
            PyUnicodeWriter_Discard(writer);
            إرجاع عدم;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    إذا (final == عدم) {
        إرجاع عدم;
    }
    إذا (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        إرجاع عدم;
    }
    إرجاع _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

ساكن asdl_تعبير_seq *
_build_concatenated_str(Parser *p, asdl_تعبير_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.قيم);
                اكسر;
            حالة TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.قيم);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_تعبير_seq* flattened = Py_asdl_تعبير_seq_new(n_flattened_elements, p->arena);
    إذا (flattened == عدم) {
        إرجاع عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        تعبير_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة JoinedStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.قيم); j++) {
                    تعبير_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.قيم, j);
                    إذا (subvalue == عدم) {
                        إرجاع عدم;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة TemplateStr_kind:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.قيم); j++) {
                    تعبير_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.قيم, j);
                    إذا (subvalue == عدم) {
                        إرجاع عدم;
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
        تعبير_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        إذا (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.قيمة) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.قيمة) == 0)
            استمر;

        إذا (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_تعبير_seq* قيم = Py_asdl_تعبير_seq_new(n_elements, p->arena);
    إذا (قيم == عدم) {
        إرجاع عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        تعبير_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        إذا (elem->kind == Constant_kind) {
            إذا (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                تعبير_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                إذا (writer == عدم) {
                    إرجاع عدم;
                }
                تعبير_ty last_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    تعبير_ty current_elem = asdl_seq_GET(flattened, j);
                    إذا (current_elem->kind == Constant_kind) {
                        إذا (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.قيمة)) {
                            PyUnicodeWriter_Discard(writer);
                            إرجاع عدم;
                        }
                        last_elem = current_elem;
                    } وإلا {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                إذا (concat_str == عدم) {
                    إرجاع عدم;
                }
                إذا (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    إرجاع عدم;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                إذا (elem == عدم) {
                    إرجاع عدم;
                }
            }

            /* Drop all empty contanst strings */
            إذا (PyUnicode_CheckExact(elem->v.Constant.قيمة) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.قيمة) == 0) {
                استمر;
            }
        }

        asdl_seq_SET(قيم, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    إرجاع قيم;
}

ساكن تعبير_ty
_build_concatenated_joined_str(Parser *p, asdl_تعبير_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_تعبير_seq *قيم = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    إذا (!قيم) {
        إرجاع عدم;
    }
    إرجاع _PyAST_JoinedStr(قيم, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

تعبير_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_تعبير_seq *strings,
                               صحيح lineno, صحيح col_offset, صحيح end_lineno,
                               صحيح end_col_offset, PyArena *arena)
{
    asdl_تعبير_seq *قيم = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    إذا (!قيم) {
        إرجاع عدم;
    }
    إرجاع _PyAST_TemplateStr(قيم, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

تعبير_ty
_PyPegen_concatenate_strings(Parser *p, asdl_تعبير_seq *strings,
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
        تعبير_ty elem = asdl_seq_GET(strings, i);
        بدل(elem->kind) {
            حالة Constant_kind:
                إذا (PyBytes_CheckExact(elem->v.Constant.قيمة)) {
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
        إرجاع عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    إذا (!f_string_found) {
        إذا (len == 1) {
            إرجاع asdl_seq_GET(strings, 0);
        }
        وإلا إذا (bytes_found) {
            إرجاع _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        وإلا {
            إرجاع _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    إرجاع _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

ساكن صحيح
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, تعبير_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    إذا (p->call_invalid_rules) {
        إرجاع 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    إذا (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        إرجاع 0;
    }

    صحيح تعداد_2 = Py_Pegen_seq_تعداد_dots(dots);
    حرف *buf = PyMem_RawMalloc(تعداد_2 + 1);
    إذا (buf == عدم) {
        PyErr_NoMemory();
        إرجاع -1;
    }
    memset(buf, '.', تعداد_2);
    buf[تعداد_2] = '\0';

    PyObject *رسالة = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    إذا (رسالة == عدم) {
        إرجاع -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحيح res = _PyErr_EmitSyntaxWarning(رسالة,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(رسالة);
    إرجاع res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, تعبير_ty module_name,
                             asdl_alias_seq *names, تعبير_ty lazy_token, صحيح lineno,
                             صحيح col_offset, صحيح end_lineno, صحيح end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = Py_Pegen_seq_تعداد_dots(dots);
    إذا (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        إذا (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            إرجاع عدم;
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
            إرجاع عدم;
        }
    }
    إرجاع _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    إذا (!p->call_invalid_rules) {
        إرجاع stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    إذا (len == 0) {
        إرجاع stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    إذا (p->last_stmt_location.lineno > last_stmt->lineno) {
        إرجاع stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    إرجاع stmts;
}
