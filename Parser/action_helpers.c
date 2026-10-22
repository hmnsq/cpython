#شامل <Python.h>
#شامل "pycore_pystate.h"         // _PyInterpreterState_GET()
#شامل "pycore_runtime.h"         // _PyRuntime
#شامل "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#شامل "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#شامل "pegen.h"
#شامل "string_parser.h"          // _PyPegen_decode_string()
#شامل "lexer/state.h"            // tok_state


خالی *
_PyPegen_dummy_name(Parser *p, ...)
{
    واپس &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, خالی *a)
{
    أكد(a != تہی);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    اگر (!seq) {
        واپس تہی;
    }
    asdl_seq_سیٹ_UNTYPED(seq, 0, a);
    واپس seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_داخل_کرو_اندر_front(Parser *p, خالی *a, asdl_seq *seq)
{
    أكد(a != تہی);
    اگر (!seq) {
        واپس _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }

    asdl_seq_سیٹ_UNTYPED(new_seq, 0, a);
    برائے (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    واپس new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_مقصد_انتہا(Parser *p, asdl_seq *seq, خالی *a)
{
    أكد(a != تہی);
    اگر (!seq) {
        واپس _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }

    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_سیٹ_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    واپس new_seq;
}

ساکن Py_ssize_t
get_flattened_seq_سائز(asdl_seq *seqs)
{
    Py_ssize_t سائز = 0;
    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        سائز += asdl_seq_LEN(inner_seq);
    }
    واپس سائز;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_سائز = get_flattened_seq_سائز(seqs);
    أكد(flattened_seq_سائز > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_سائز, p->arena);
    اگر (!flattened_seq) {
        واپس تہی;
    }

    صحیح flattened_seq_idx = 0;
    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        برائے (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_سیٹ_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_سائز);

    واپس flattened_seq;
}

خالی *
Py_Pegen_seq_آخری_شے(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    واپس asdl_seq_GET_UNTYPED(seq, len - 1);
}

خالی *
Py_Pegen_seq_پہلا_شے(asdl_seq *seq)
{
    واپس asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
اظہاریہ_ty
Py_Pegen_join_نام_with_dot(Parser *p, اظہاریہ_ty پہلا_name, اظہاریہ_ty دوسرا_name)
{
    أكد(پہلا_name != تہی && دوسرا_name != تہی);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            پہلا_name->v.Name.id, دوسرا_name->v.Name.id);
    اگر (!uni) {
        واپس تہی;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        واپس تہی;
    }

    واپس _PyAST_Name(uni, بھرو, EXTRA_اظہاریہ(پہلا_name, دوسرا_name));
}

/* Counts the total number of dots in seq's tokens */
صحیح
Py_Pegen_seq_گنتی_dots(asdl_seq *seq)
{
    صحیح نمبر_کا_dots = 0;
    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_اظہاریہ = asdl_seq_GET_UNTYPED(seq, i);
        انتخاب (current_اظہاریہ->type) {
            حالت ELLIPSIS:
                نمبر_کا_dots += 3;
                توڑو;
            حالت DOT:
                نمبر_کا_dots += 1;
                توڑو;
            طےشدہ:
                Py_UNREACHABLE();
        }
    }

    واپس نمبر_کا_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_ستارہ(Parser *p, صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                        صحیح انتہا_ستون_انحراف, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    اگر (!str) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        واپس تہی;
    }
    واپس _PyAST_alias(str, تہی, lineno, ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_نام_مقصد_ids(Parser *p, asdl_اظہاریہ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_سیٹ(new_seq, i, e->v.Name.id);
    }
    واپس new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_اظہاریہ_Pair *
Py_Pegen_cmpop_اظہاریہ_pair(Parser *p, cmpop_ty cmpop, اظہاریہ_ty اظہاریہ)
{
    أكد(اظہاریہ != تہی);
    Cmpop_اظہاریہ_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_اظہاریہ_Pair));
    اگر (!a) {
        واپس تہی;
    }
    a->cmpop = cmpop;
    a->اظہاریہ = اظہاریہ;
    واپس a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_اظہاریہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->cmpop);
    }
    واپس new_seq;
}

asdl_اظہاریہ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_اظہاریہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->اظہاریہ);
    }
    واپس new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساکن asdl_اظہاریہ_seq *
سیٹ_seq_context(Parser *p, asdl_اظہاریہ_seq *seq, اظہاریہ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    اگر (len == 0) {
        واپس تہی;
    }

    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty e = asdl_seq_GET(seq, i);
        اظہاریہ_ty new_e = Py_Pegen_سیٹ_اظہاریہ_context(p, e, ctx);
        اگر (!new_e) {
            واپس تہی;
        }
        asdl_seq_سیٹ(new_seq, i, new_e);
    }
    واپس new_seq;
}

ساکن اظہاریہ_ty
سیٹ_name_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    واپس _PyAST_Name(e->v.Name.id, ctx, EXTRA_اظہاریہ(e, e));
}

ساکن اظہاریہ_ty
سیٹ_tuple_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    asdl_اظہاریہ_seq *seq = سیٹ_seq_context(p, e->v.Tuple.elts, ctx);
    اگر (!seq && PyErr_Occurred()) {
        واپس تہی;
    }
    واپس _PyAST_Tuple(seq, ctx, EXTRA_اظہاریہ(e, e));
}

ساکن اظہاریہ_ty
سیٹ_list_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    asdl_اظہاریہ_seq *seq = سیٹ_seq_context(p, e->v.List.elts, ctx);
    اگر (!seq && PyErr_Occurred()) {
        واپس تہی;
    }
    واپس _PyAST_List(seq, ctx, EXTRA_اظہاریہ(e, e));
}

ساکن اظہاریہ_ty
سیٹ_subscript_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    واپس _PyAST_Subscript(e->v.Subscript.قدر, e->v.Subscript.قاش,
                            ctx, EXTRA_اظہاریہ(e, e));
}

ساکن اظہاریہ_ty
سیٹ_attribute_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    واپس _PyAST_Attribute(e->v.Attribute.قدر, e->v.Attribute.attr,
                            ctx, EXTRA_اظہاریہ(e, e));
}

ساکن اظہاریہ_ty
سیٹ_starred_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    اظہاریہ_ty inner = Py_Pegen_سیٹ_اظہاریہ_context(p, e->v.Starred.قدر, ctx);
    اگر (!inner) {
        واپس تہی;
    }
    واپس _PyAST_Starred(inner, ctx, EXTRA_اظہاریہ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
اظہاریہ_ty
Py_Pegen_سیٹ_اظہاریہ_context(Parser *p, اظہاریہ_ty اظہاریہ, اظہاریہ_context_ty ctx)
{
    أكد(اظہاریہ != تہی);

    اظہاریہ_ty new = تہی;
    انتخاب (اظہاریہ->قسم) {
        حالت Name_قسم:
            new = سیٹ_name_context(p, اظہاریہ, ctx);
            توڑو;
        حالت Tuple_قسم:
            new = سیٹ_tuple_context(p, اظہاریہ, ctx);
            توڑو;
        حالت List_قسم:
            new = سیٹ_list_context(p, اظہاریہ, ctx);
            توڑو;
        حالت Subscript_قسم:
            new = سیٹ_subscript_context(p, اظہاریہ, ctx);
            توڑو;
        حالت Attribute_قسم:
            new = سیٹ_attribute_context(p, اظہاریہ, ctx);
            توڑو;
        حالت Starred_قسم:
            new = سیٹ_starred_context(p, اظہاریہ, ctx);
            توڑو;
        طےشدہ:
            new = اظہاریہ;
    }
    واپس new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
اظہاریہ_ty
Py_Pegen_make_ہدف(Parser *p, اظہاریہ_ty اظہاریہ, TARGETS_TYPE targets_type)
{
    أكد(اظہاریہ != تہی);
    أكد(targets_type != FOR_TARGETS);
    اگر (Py_Pegen_get_invalid_ہدف(اظہاریہ, targets_type) != تہی) {
        واپس تہی;
    }
    واپس Py_Pegen_سیٹ_اظہاریہ_context(
        p, اظہاریہ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
کلید_قدر_Pair *
Py_Pegen_کلید_قدر_pair(Parser *p, اظہاریہ_ty کلید, اظہاریہ_ty قدر)
{
    کلید_قدر_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_قدر_Pair));
    اگر (!a) {
        واپس تہی;
    }
    a->کلید = کلید;
    a->قدر = قدر;
    واپس a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_کلیدیں(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        کلید_قدر_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->کلید);
    }
    واپس new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_اقدار(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        کلید_قدر_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->قدر);
    }
    واپس new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
کلید_نمونہ_Pair *
Py_Pegen_کلید_نمونہ_pair(Parser *p, اظہاریہ_ty کلید, نمونہ_ty نمونہ)
{
    کلید_نمونہ_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_نمونہ_Pair));
    اگر (!a) {
        واپس تہی;
    }
    a->کلید = کلید;
    a->نمونہ = نمونہ;
    واپس a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_نمونہ_کلیدیں(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        کلید_نمونہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->کلید);
    }
    واپس new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_نمونہ_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_نمونہ_seq *new_seq = Py_asdl_نمونہ_seq_new(len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        کلید_نمونہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->نمونہ);
    }
    واپس new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, اظہاریہ_ty قدر, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    اگر (!a) {
        واپس تہی;
    }
    a->arg = Py_Pegen_جوڑو_type_comment_مقصد_arg(p, arg, tc);
    اگر (!a->arg) {
        واپس تہی;
    }
    a->قدر = قدر;
    واپس a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_نام, asdl_seq *نام_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    اگر (!a) {
        واپس تہی;
    }
    a->plain_نام = plain_نام;
    a->نام_with_defaults = نام_with_defaults;
    واپس a;
}

/* Constructs a StarEtc */
ستارہ_Etc *
Py_Pegen_ستارہ_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    ستارہ_Etc *a = _PyArena_Malloc(p->arena, حجم(ستارہ_Etc));
    اگر (!a) {
        واپس تہی;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    واپس a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t پہلا_len = asdl_seq_LEN(a);
    Py_ssize_t دوسرا_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(پہلا_len + دوسرا_len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }

    صحیح k = 0;
    برائے (Py_ssize_t i = 0; i < پہلا_len; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    برائے (Py_ssize_t i = 0; i < دوسرا_len; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    واپس new_seq;
}

ساکن asdl_arg_seq*
get_نام(Parser *p, asdl_seq *نام_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نام_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    اگر (!seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نام_with_defaults, i);
        asdl_seq_سیٹ(seq, i, pair->arg);
    }
    واپس seq;
}

ساکن asdl_اظہاریہ_seq *
_get_defaults(Parser *p, asdl_seq *نام_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نام_with_defaults);
    asdl_اظہاریہ_seq *seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    اگر (!seq) {
        واپس تہی;
    }
    برائے (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نام_with_defaults, i);
        asdl_seq_سیٹ(seq, i, pair->قدر);
    }
    واپس seq;
}

ساکن صحیح
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    اگر (slash_without_default != تہی) {
        *posonlyargs = slash_without_default;
    }
    ورنہ اگر (slash_with_default != تہی) {
        asdl_arg_seq *slash_with_default_نام =
                get_نام(p, slash_with_default->نام_with_defaults);
        اگر (!slash_with_default_نام) {
            واپس -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_نام,
                (asdl_seq*)slash_with_default_نام);
    }
    ورنہ {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    واپس *posonlyargs == تہی ? -1 : 0;
}

ساکن صحیح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_نام,
              asdl_seq *نام_with_default,
              asdl_arg_seq **posargs) {

    اگر (نام_with_default != تہی) {
        اگر (plain_نام != تہی) {
            asdl_arg_seq *نام_with_default_نام = get_نام(p, نام_with_default);
            اگر (!نام_with_default_نام) {
                واپس -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_نام, (asdl_seq*)نام_with_default_نام);
        }
        ورنہ {
            *posargs = get_نام(p, نام_with_default);
        }
    }
    ورنہ {
        اگر (plain_نام != تہی) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_نام;
        }
        ورنہ {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    واپس *posargs == تہی ? -1 : 0;
}

ساکن صحیح
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *نام_with_default,
                  asdl_اظہاریہ_seq **posdefaults) {
    اگر (slash_with_default != تہی && نام_with_default != تہی) {
        asdl_اظہاریہ_seq *slash_with_default_اقدار =
                _get_defaults(p, slash_with_default->نام_with_defaults);
        اگر (!slash_with_default_اقدار) {
            واپس -1;
        }
        asdl_اظہاریہ_seq *نام_with_default_اقدار = _get_defaults(p, نام_with_default);
        اگر (!نام_with_default_اقدار) {
            واپس -1;
        }
        *posdefaults = (asdl_اظہاریہ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_اقدار,
                (asdl_seq*)نام_with_default_اقدار);
    }
    ورنہ اگر (slash_with_default == تہی && نام_with_default != تہی) {
        *posdefaults = _get_defaults(p, نام_with_default);
    }
    ورنہ اگر (slash_with_default != تہی && نام_with_default == تہی) {
        *posdefaults = _get_defaults(p, slash_with_default->نام_with_defaults);
    }
    ورنہ {
        *posdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    }
    واپس *posdefaults == تہی ? -1 : 0;
}

ساکن صحیح
_make_kwargs(Parser *p, ستارہ_Etc *ستارہ_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_اظہاریہ_seq **kwdefaults) {
    اگر (ستارہ_etc != تہی && ستارہ_etc->kwonlyargs != تہی) {
        *kwonlyargs = get_نام(p, ستارہ_etc->kwonlyargs);
    }
    ورنہ {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    اگر (*kwonlyargs == تہی) {
        واپس -1;
    }

    اگر (ستارہ_etc != تہی && ستارہ_etc->kwonlyargs != تہی) {
        *kwdefaults = _get_defaults(p, ستارہ_etc->kwonlyargs);
    }
    ورنہ {
        *kwdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    }

    اگر (*kwdefaults == تہی) {
        واپس -1;
    }

    واپس 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_نام,
                        asdl_seq *نام_with_default, ستارہ_Etc *ستارہ_etc)
{
    asdl_arg_seq *posonlyargs;
    اگر (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        واپس تہی;
    }

    asdl_arg_seq *posargs;
    اگر (_make_posargs(p, plain_نام, نام_with_default, &posargs) == -1) {
        واپس تہی;
    }

    asdl_اظہاریہ_seq *posdefaults;
    اگر (_make_posdefaults(p,slash_with_default, نام_with_default, &posdefaults) == -1) {
        واپس تہی;
    }

    arg_ty vararg = تہی;
    اگر (ستارہ_etc != تہی && ستارہ_etc->vararg != تہی) {
        vararg = ستارہ_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_اظہاریہ_seq *kwdefaults;
    اگر (_make_kwargs(p, ستارہ_etc, &kwonlyargs, &kwdefaults) == -1) {
        واپس تہی;
    }

    arg_ty kwarg = تہی;
    اگر (ستارہ_etc != تہی && ستارہ_etc->kwarg != تہی) {
        kwarg = ستارہ_etc->kwarg;
    }

    واپس _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_خالی_خانے_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!posonlyargs) {
        واپس تہی;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!posargs) {
        واپس تہی;
    }
    asdl_اظہاریہ_seq *posdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    اگر (!posdefaults) {
        واپس تہی;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    اگر (!kwonlyargs) {
        واپس تہی;
    }
    asdl_اظہاریہ_seq *kwdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    اگر (!kwdefaults) {
        واپس تہی;
    }

    واپس _PyAST_arguments(posonlyargs, posargs, تہی, kwonlyargs,
                            kwdefaults, تہی, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty قسم)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    اگر (!a) {
        واپس تہی;
    }
    a->قسم = قسم;
    واپس a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_اظہاریہ_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != تہی);
    اگر (function_def->قسم == Async_Function_Def_قسم) {
        واپس _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.دلائل,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->ستون_انحراف,
            function_def->انتہا_lineno, function_def->انتہا_ستون_انحراف, p->arena);
    }

    واپس _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.دلائل,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->ستون_انحراف,
        function_def->انتہا_lineno, function_def->انتہا_ستون_انحراف, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_اظہاریہ_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != تہی);
    واپس _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->ستون_انحراف, class_def->انتہا_lineno,
        class_def->انتہا_ستون_انحراف, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, خالی *element, صحیح is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, حجم(KeywordOrStarred));
    اگر (!a) {
        واپس تہی;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    واپس a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
ساکن صحیح
seq_نمبر_کا_starred_exprs(asdl_seq *seq)
{
    صحیح n = 0;
    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        اگر (!k->is_keyword) {
            n++;
        }
    }
    واپس n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_اظہاریہ_seq *
Py_Pegen_seq_نکالو_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحیح new_len = seq_نمبر_کا_starred_exprs(kwargs);
    اگر (new_len == 0) {
        واپس تہی;
    }
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(new_len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }

    صحیح idx = 0;
    برائے (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        اگر (!k->is_keyword) {
            asdl_seq_سیٹ(new_seq, idx++, k->element);
        }
    }
    واپس new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_نمبر_کا_starred_exprs(kwargs);
    اگر (new_len == 0) {
        واپس تہی;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    اگر (!new_seq) {
        واپس تہی;
    }

    صحیح idx = 0;
    برائے (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        اگر (k->is_keyword) {
            asdl_seq_سیٹ(new_seq, idx++, k->element);
        }
    }
    واپس new_seq;
}

اظہاریہ_ty
_PyPegen_ensure_imaginary(Parser *p, اظہاریہ_ty قوت_نما)
{
    اگر (قوت_نما->قسم != Constant_قسم || !PyComplex_CheckExact(قوت_نما->v.Constant.قدر)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(قوت_نما, "imaginary number required in complex literal");
        واپس تہی;
    }
    واپس قوت_نما;
}

اظہاریہ_ty
_PyPegen_ensure_real(Parser *p, اظہاریہ_ty قوت_نما)
{
    اگر (قوت_نما->قسم != Constant_قسم || PyComplex_CheckExact(قوت_نما->v.Constant.قدر)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(قوت_نما, "real number required in complex literal");
        واپس تہی;
    }
    واپس قوت_نما;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = تہی;
    Py_ssize_t عدد = p->type_ignore_comments.عدد_اشیاء;
    اگر (عدد > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(عدد, p->arena);
        اگر (type_ignores == تہی) {
            واپس تہی;
        }
        برائے (Py_ssize_t i = 0; i < عدد; i++) {
            PyObject *لیبل = _PyPegen_new_type_comment(p, p->type_ignore_comments.اشیاء[i].comment);
            اگر (لیبل == تہی) {
                واپس تہی;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.اشیاء[i].lineno,
                                                  لیبل, p->arena);
            اگر (ti == تہی) {
                واپس تہی;
            }
            asdl_seq_سیٹ(type_ignores, i, ti);
        }
    }
    واپس _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, لڑی_لمبائی(s), تہی);
    اگر (res == تہی) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        واپس تہی;
    }
    واپس res;
}

arg_ty
Py_Pegen_جوڑو_type_comment_مقصد_arg(Parser *p, arg_ty a, Token *tc)
{
    اگر (tc == تہی) {
        واپس a;
    }
    ثابت حرف *بائٹیں = Py_بائٹیں_As_String(tc->بائٹیں);
    اگر (بائٹیں == تہی) {
        واپس تہی;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بائٹیں);
    اگر (tco == تہی) {
        واپس تہی;
    }
    واپس _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->ستون_انحراف, a->انتہا_lineno, a->انتہا_ستون_انحراف,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحیح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->بائٹیں != تہی);
    أكد(t->type == NOTEQUAL);

    ثابت حرف* tok_str = Py_بائٹیں_AS_STRING(t->بائٹیں);
    اگر (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        واپس -1;
    }
    اگر (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        واپس strcmp(tok_str, "!=");
    }
    واپس 0;
}

صحیح
_PyPegen_check_legacy_stmt(Parser *p, اظہاریہ_ty name) {
    اگر (name->قسم != Name_قسم) {
        واپس 0;
    }
    ثابت حرف* candidates[2] = {"print", "exec"};
    برائے (صحیح i=0; i<2; i++) {
        اگر (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            واپس 1;
        }
    }
    واپس 0;
}

خالی *
_PyPegen_raise_error_for_missing_comma(Parser *p, اظہاریہ_ty a, اظہاریہ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    اگر (_PyPegen_check_legacy_stmt(p, a)) {
        واپس تہی;
    }
    // Only raise inside parentheses/brackets (level > 0)
    اگر (p->علامات[p->mark - 1]->level == 0) {
        واپس تہی;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    اگر (a->انتہا_lineno > a->lineno) {
        واپس RAISE_ERROR_معلوم_LOCATION(
            p, PyExc_SyntaxError, a->انتہا_lineno, a->ستون_انحراف,
            a->انتہا_lineno, a->انتہا_ستون_انحراف,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    واپس RAISE_ERROR_معلوم_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->ستون_انحراف,
        b->انتہا_lineno, b->انتہا_ستون_انحراف,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساکن نتیجہ_Token_With_Metadata *
نتیجہ_token_with_metadata(Parser *p, خالی *نتیجہ, PyObject *metadata)
{
    نتیجہ_Token_With_Metadata *res = _PyArena_Malloc(p->arena, حجم(نتیجہ_Token_With_Metadata));
    اگر (res == تہی) {
        واپس تہی;
    }
    res->metadata = metadata;
    res->نتیجہ = نتیجہ;
    واپس res;
}

ساکن حرف
formatted_string_سابقہ(ثابت Parser *p)
{
    صحیح nested = 0;
    برائے (صحیح i = p->mark - 1; i >= 0; i--) {
        صحیح type = p->علامات[i]->type;
        اگر (type == FSTRING_انتہا || type == TSTRING_انتہا) {
            nested++;
        }
        ورنہ اگر (type == FSTRING_آغاز || type == TSTRING_آغاز) {
            اگر (nested == 0) {
                واپس type == TSTRING_آغاز ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

نتیجہ_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, اظہاریہ_ty conv)
{
    اگر (conv_token->lineno != conv->lineno || conv_token->انتہا_ستون_انحراف != conv->ستون_انحراف) {
        واپس RAISE_SYNTAX_ERROR_معلوم_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_سابقہ(p)
        );
    }

    Py_UCS4 پہلا = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    اگر (Py_Unicode_GET_لمبائی(conv->v.Name.id) > 1 ||
            !(پہلا == 's' || پہلا == 'r' || پہلا == 'a')) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_سابقہ(p),
                                            conv->v.Name.id);
        واپس تہی;
    }

    واپس نتیجہ_token_with_metadata(p, conv, conv_token->metadata);
}

نتیجہ_Token_With_Metadata *
Py_Pegen_تیاری_بھرا_format_spec(Parser *p, Token *colon, asdl_اظہاریہ_seq *spec, صحیح lineno, صحیح ستون_انحراف,
                                صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف, PyArena *arena)
{
    اگر (!spec) {
        واپس تہی;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_اشیاء = asdl_seq_LEN(spec);
    Py_ssize_t non_خالی_خانے_گنتی = 0;
    برائے (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(spec, i);
        non_خالی_خانے_گنتی += !(شے->قسم == Constant_قسم &&
                             PyUnicode_CheckExact(شے->v.Constant.قدر) &&
                             Py_Unicode_GET_لمبائی(شے->v.Constant.قدر) == 0);
    }
    اگر (non_خالی_خانے_گنتی != n_اشیاء) {
        asdl_اظہاریہ_seq *resized_spec =
            Py_asdl_اظہاریہ_seq_new(non_خالی_خانے_گنتی, p->arena);
        اگر (resized_spec == تہی) {
            واپس تہی;
        }
        Py_ssize_t j = 0;
        برائے (Py_ssize_t i = 0; i < n_اشیاء; i++) {
            اظہاریہ_ty شے = asdl_seq_GET(spec, i);
            اگر (شے->قسم == Constant_قسم &&
                PyUnicode_CheckExact(شے->v.Constant.قدر) &&
                Py_Unicode_GET_لمبائی(شے->v.Constant.قدر) == 0) {
                جاری;
            }
            asdl_seq_سیٹ(resized_spec, j++, شے);
        }
        أكد(j == non_خالی_خانے_گنتی);
        spec = resized_spec;
    }
    اظہاریہ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    اگر (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->قسم == Constant_قسم)) {
        res = _PyAST_JoinedStr(spec, lineno, ستون_انحراف, انتہا_lineno,
                                    انتہا_ستون_انحراف, p->arena);
    } ورنہ {
        res = Py_Pegen_concatenate_لڑیاں(p, spec,
                             lineno, ستون_انحراف, انتہا_lineno,
                             انتہا_ستون_انحراف, arena);
    }
    اگر (!res) {
        واپس تہی;
    }
    واپس نتیجہ_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_اظہاریہ_name(اظہاریہ_ty e)
{
    أكد(e != تہی);
    انتخاب (e->قسم) {
        حالت Attribute_قسم:
            واپس "attribute";
        حالت Subscript_قسم:
            واپس "subscript";
        حالت Starred_قسم:
            واپس "starred";
        حالت Name_قسم:
            واپس "name";
        حالت List_قسم:
            واپس "list";
        حالت Tuple_قسم:
            واپس "tuple";
        حالت Lambda_قسم:
            واپس "lambda";
        حالت Call_قسم:
            واپس "function call";
        حالت Bool_Op_قسم:
        حالت Bin_Op_قسم:
        حالت Unary_Op_قسم:
            واپس "expression";
        حالت Generator_Exp_قسم:
            واپس "generator expression";
        حالت Yield_قسم:
        حالت Yield_From_قسم:
            واپس "yield expression";
        حالت Await_قسم:
            واپس "await expression";
        حالت List_Comp_قسم:
            واپس "list comprehension";
        حالت سیٹ_Comp_قسم:
            واپس "set comprehension";
        حالت Dict_Comp_قسم:
            واپس "dict comprehension";
        حالت Dict_قسم:
            واپس "dict literal";
        حالت سیٹ_قسم:
            واپس "set display";
        حالت Joined_Str_قسم:
        حالت Formatted_قدر_قسم:
            واپس "f-string expression";
        حالت Template_Str_قسم:
        حالت Interpolation_قسم:
            واپس "t-string expression";
        حالت Constant_قسم: {
            PyObject *قدر = e->v.Constant.قدر;
            اگر (قدر == Py_None) {
                واپس "None";
            }
            اگر (قدر == Py_False) {
                واپس "False";
            }
            اگر (قدر == Py_True) {
                واپس "True";
            }
            اگر (قدر == Py_Ellipsis) {
                واپس "ellipsis";
            }
            واپس "literal";
        }
        حالت Compare_قسم:
            واپس "comparison";
        حالت If_Exp_قسم:
            واپس "conditional expression";
        حالت Named_اظہاریہ_قسم:
            واپس "named expression";
        طےشدہ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->قسم, e->lineno);
            واپس تہی;
    }
}

اظہاریہ_ty
Py_Pegen_get_آخری_comprehension_شے(comprehension_ty comprehension) {
    اگر (comprehension->ifs == تہی || asdl_seq_LEN(comprehension->ifs) == 0) {
        واپس comprehension->iter;
    }
    واپس Py_Pegen_آخری_شے(comprehension->ifs, اظہاریہ_ty);
}

اظہاریہ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_اظہاریہ_seq *a, asdl_seq *b,
                     صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                     صحیح انتہا_ستون_انحراف, PyArena *arena) {
    Py_ssize_t دلائل_len = asdl_seq_LEN(a);
    Py_ssize_t کل_len = دلائل_len;

    اگر (b == تہی) {
        واپس _PyAST_Call(_PyPegen_dummy_name(p), a, تہی, lineno, ستون_انحراف,
                        انتہا_lineno, انتہا_ستون_انحراف, arena);

    }

    asdl_اظہاریہ_seq *starreds = Py_Pegen_seq_نکالو_starred_exprs(p, b);
    اگر (!starreds && PyErr_Occurred()) {
        واپس تہی;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    اگر (!keywords && PyErr_Occurred()) {
        واپس تہی;
    }

    اگر (starreds) {
        کل_len += asdl_seq_LEN(starreds);
    }

    asdl_اظہاریہ_seq *دلائل = Py_asdl_اظہاریہ_seq_new(کل_len, arena);
    اگر (دلائل == تہی) {
        واپس تہی;
    }

    Py_ssize_t i = 0;
    برائے (i = 0; i < دلائل_len; i++) {
        asdl_seq_سیٹ(دلائل, i, asdl_seq_GET(a, i));
    }
    برائے (; i < کل_len; i++) {
        asdl_seq_سیٹ(دلائل, i, asdl_seq_GET(starreds, i - دلائل_len));
    }

    واپس _PyAST_Call(_PyPegen_dummy_name(p), دلائل, keywords, lineno,
                       ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
}

// AST Error reporting helpers

اظہاریہ_ty
Py_Pegen_get_invalid_ہدف(اظہاریہ_ty e, TARGETS_TYPE targets_type)
{
    اگر (e == تہی) {
        واپس تہی;
    }

    اگر (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        اگر (targets_type == SINGLE_TARGETS && e->قسم == Name_قسم) {
            واپس تہی;
        }
        واپس e->قسم == Attribute_قسم || e->قسم == Subscript_قسم ? تہی : e;
    }

#تعریف_کرو دورہ_CONTAINER(CONTAINER, TYPE) کرو { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        برائے (Py_ssize_t i = 0; i < len; i++) {\
            اظہاریہ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            اظہاریہ_ty بچہ = Py_Pegen_get_invalid_ہدف(other, targets_type);\
            اگر (بچہ != تہی) {\
                واپس بچہ;\
            }\
        }\
    } جب (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    انتخاب (e->قسم) {
        حالت List_قسم:
            دورہ_CONTAINER(e, List);
            واپس تہی;
        حالت Tuple_قسم:
            دورہ_CONTAINER(e, Tuple);
            واپس تہی;
        حالت Starred_قسم:
            اگر (targets_type == DEL_TARGETS) {
                واپس e;
            }
            واپس Py_Pegen_get_invalid_ہدف(e->v.Starred.قدر, targets_type);
        حالت Compare_قسم:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            اگر (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                اگر (cmpop == اندر) {
                    واپس Py_Pegen_get_invalid_ہدف(e->v.Compare.بایاں, targets_type);
                }
                واپس تہی;
            }
            واپس e;
        حالت Name_قسم:
        حالت Subscript_قسم:
        حالت Attribute_قسم:
            واپس تہی;
        طےشدہ:
            واپس e;
    }
}

خالی *_PyPegen_arguments_parsing_error(Parser *p, اظہاریہ_ty e) {
    صحیح kwarg_unpacking = 0;
    برائے (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        اگر (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *پیغام = تہی;
    اگر (kwarg_unpacking) {
        پیغام = "positional argument follows keyword argument unpacking";
    } ورنہ {
        پیغام = "positional argument follows keyword argument";
    }

    واپس RAISE_SYNTAX_ERROR(پیغام);
}

خالی *
Py_Pegen_nonparen_genexp_اندر_call(Parser *p, اظہاریہ_ty دلائل, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(دلائل->v.Call.دلائل);
    اگر (len <= 1) {
        واپس تہی;
    }

    comprehension_ty آخری_comprehension = Py_Pegen_آخری_شے(comprehensions, comprehension_ty);

    واپس RAISE_SYNTAX_ERROR_معلوم_RANGE(
        (اظہاریہ_ty) asdl_seq_GET(دلائل->v.Call.دلائل, len - 1),
        Py_Pegen_get_آخری_comprehension_شے(آخری_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساکن اظہاریہ_ty
_PyPegen_decode_fstring_part(Parser* p, صحیح is_raw, اظہاریہ_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.قدر));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.قدر);
    اگر (bstr == تہی) {
        واپس تہی;
    }

    سائز_t len;
    اگر (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } ورنہ {
        len = لڑی_لمبائی(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == تہی;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    اگر (str == تہی) {
        _Pypegen_raise_decode_error(p);
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        واپس تہی;
    }
    واپس _PyAST_Constant(str, تہی, constant->lineno, constant->ستون_انحراف,
                           constant->انتہا_lineno, constant->انتہا_ستون_انحراف,
                           p->arena);
}

ساکن asdl_اظہاریہ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions,
                   Token *b, صحیح is_tstring)
{
    Py_ssize_t n_اشیاء = asdl_seq_LEN(raw_expressions);
    Py_ssize_t کل_اشیاء = n_اشیاء;
    برائے (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(raw_expressions, i);
        اگر (شے->قسم == Joined_Str_قسم) {
            کل_اشیاء += asdl_seq_LEN(شے->v.JoinedStr.اقدار) - 1;
        }
    }

    ثابت حرف* quote_str = Py_بائٹیں_As_String(a->بائٹیں);
    اگر (quote_str == تہی) {
        واپس تہی;
    }
    صحیح is_raw = strpbrk(quote_str, "rR") != تہی;

    asdl_اظہاریہ_seq *seq = Py_asdl_اظہاریہ_seq_new(کل_اشیاء, p->arena);
    اگر (seq == تہی) {
        واپس تہی;
    }

    Py_ssize_t اشاریہ = 0;
    برائے (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        اگر (شے->قسم == Joined_Str_قسم) {
            asdl_اظہاریہ_seq *اقدار = شے->v.JoinedStr.اقدار;
            اگر (asdl_seq_LEN(اقدار) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             شے->lineno);
                واپس تہی;
            }

            اظہاریہ_ty پہلا = asdl_seq_GET(اقدار, 0);
            أكد(پہلا->قسم == Constant_قسم);
            asdl_seq_سیٹ(seq, اشاریہ++, پہلا);

            اظہاریہ_ty دوسرا = asdl_seq_GET(اقدار, 1);
            أكد((is_tstring &&
                    دوسرا->قسم == Interpolation_قسم) ||
                   دوسرا->قسم == Formatted_قدر_قسم);
            asdl_seq_سیٹ(seq, اشاریہ++, دوسرا);

            جاری;
        }

        اگر (شے->قسم == Constant_قسم) {
            شے = _PyPegen_decode_fstring_part(p, is_raw, شے, b);
            اگر (شے == تہی) {
                واپس تہی;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            اگر (PyUnicode_CheckExact(شے->v.Constant.قدر)
                && Py_Unicode_GET_لمبائی(شے->v.Constant.قدر) == 0) {
                جاری;
            }
        }
        asdl_seq_سیٹ(seq, اشاریہ++, شے);
    }

    asdl_اظہاریہ_seq *resized_exprs;
    اگر (اشاریہ != کل_اشیاء) {
        resized_exprs = Py_asdl_اظہاریہ_seq_new(اشاریہ, p->arena);
        اگر (resized_exprs == تہی) {
            واپس تہی;
        }
        برائے (Py_ssize_t i = 0; i < اشاریہ; i++) {
            asdl_seq_سیٹ(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    ورنہ {
        resized_exprs = seq;
    }
    واپس resized_exprs;
}

اظہاریہ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b) {

    asdl_اظہاریہ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    اگر (resized_exprs == تہی) {
        واپس تہی;
    }
    واپس _PyAST_TemplateStr(resized_exprs, a->lineno, a->ستون_انحراف,
                              b->انتہا_lineno, b->انتہا_ستون_انحراف,
                              p->arena);
}

اظہاریہ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_اظہاریہ_seq* raw_expressions, Token*b) {

    asdl_اظہاریہ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    اگر (resized_exprs == تہی) {
        واپس تہی;
    }
    واپس _PyAST_JoinedStr(resized_exprs, a->lineno, a->ستون_انحراف,
                            b->انتہا_lineno, b->انتہا_ستون_انحراف,
                            p->arena);
}

اظہاریہ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    اگر (tok->بائٹیں == تہی) {
        واپس Py_Pegen_raise_incomplete_داخلہ(p);
    }
    Py_ssize_t bsize = Py_بائٹیں_GET_سائز(tok->بائٹیں);
    ثابت حرف *bstr = Py_بائٹیں_AS_STRING(tok->بائٹیں);

    صحیح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    اگر (str == تہی) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        واپس تہی;
    }
    واپس _PyAST_Constant(str, تہی, tok->lineno, tok->ستون_انحراف,
                           tok->انتہا_lineno, tok->انتہا_ستون_انحراف,
                           p->arena);
}

اظہاریہ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    اگر (tok->بائٹیں == تہی) {
        واپس Py_Pegen_raise_incomplete_داخلہ(p);
    }
    ثابت حرف *bstr = Py_بائٹیں_AS_STRING(tok->بائٹیں);
    PyObject* str = PyUnicode_FromString(bstr);
    اگر (str == تہی) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        واپس تہی;
    }
    واپس _PyAST_Constant(str, تہی, tok->lineno, tok->ستون_انحراف,
                           tok->انتہا_lineno, tok->انتہا_ستون_انحراف,
                           p->arena);
}

اظہاریہ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    اگر (tok->بائٹیں == تہی) {
        واپس Py_Pegen_raise_incomplete_داخلہ(p);
    }
    ثابت حرف *the_str = Py_بائٹیں_AS_STRING(tok->بائٹیں);
    PyObject *s = _PyPegen_parse_string(p, tok);
    اگر (s == تہی) {
        _Pypegen_raise_decode_error(p);
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        واپس تہی;
    }
    PyObject *قسم = تہی;
    اگر (the_str && the_str[0] == 'u') {
        قسم = _PyPegen_new_identifier(p, "u");
        اگر (قسم == تہی) {
            واپس تہی;
        }
    }
    واپس _PyAST_Constant(s, قسم, tok->lineno, tok->ستون_انحراف, tok->انتہا_lineno, tok->انتہا_ستون_انحراف, p->arena);
}

ساکن صحیح
_get_interpolation_conversion(Parser *p, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                              نتیجہ_Token_With_Metadata *format)
{
    اگر (conversion != تہی) {
        اظہاریہ_ty conversion_اظہاریہ = (اظہاریہ_ty) conversion->نتیجہ;
        أكد(conversion_اظہاریہ->قسم == Name_قسم);
        Py_UCS4 پہلا = PyUnicode_READ_CHAR(conversion_اظہاریہ->v.Name.id, 0);
        واپس Py_محفوظ_DOWNCAST(پہلا, Py_UCS4, صحیح);
    }
    ورنہ اگر (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        واپس (صحیح)'r';
    }
    واپس -1;
}

ساکن PyObject *
strip_interpolation_debug_اظہاریہ(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_لمبائی(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    جب (len > 0) {
        صحیح has_newline = 0;
        جب (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            اگر (!_PyUnicode_IsWhitespace(c)) {
                توڑو;
            }
            اگر (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        اگر (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            توڑو;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    اگر (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        واپس Py_NewRef(exprstr);
    }

    واپس PyUnicode_Substring(exprstr, 0, len - 1);
}

اظہاریہ_ty _PyPegen_interpolation(Parser *p, اظہاریہ_ty عبارت, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                                 نتیجہ_Token_With_Metadata *format, Token *closing_brace, صحیح lineno, صحیح ستون_انحراف,
                                 صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف, PyArena *arena) {

    صحیح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحیح debug_انتہا_سطر, debug_انتہا_انحراف;
    PyObject *debug_metadata;
    constant exprstr;

    اگر (conversion) {
        debug_انتہا_سطر = ((اظہاریہ_ty) conversion->نتیجہ)->lineno;
        debug_انتہا_انحراف = ((اظہاریہ_ty) conversion->نتیجہ)->ستون_انحراف;
        debug_metadata = exprstr = conversion->metadata;
    }
    ورنہ اگر (format) {
        debug_انتہا_سطر = ((اظہاریہ_ty) format->نتیجہ)->lineno;
        debug_انتہا_انحراف = ((اظہاریہ_ty) format->نتیجہ)->ستون_انحراف + 1;
        debug_metadata = exprstr = format->metadata;
    }
    ورنہ {
        debug_انتہا_سطر = انتہا_lineno;
        debug_انتہا_انحراف = انتہا_ستون_انحراف;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != تہی);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_اظہاریہ(exprstr)
        : Py_NewRef(exprstr);
    اگر (!final_exprstr || Py_Arena_جوڑو_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        واپس تہی;
    }

    اظہاریہ_ty interpolation = _PyAST_Interpolation(
        عبارت, final_exprstr, conversion_val, format ? (اظہاریہ_ty) format->نتیجہ : تہی,
        lineno, ستون_انحراف, انتہا_lineno,
        انتہا_ستون_انحراف, arena
    );

    اگر (!interpolation || !debug) {
        واپس interpolation;
    }

    اظہاریہ_ty debug_متن = _PyAST_Constant(debug_metadata, تہی, lineno, ستون_انحراف + 1, debug_انتہا_سطر,
                                            debug_انتہا_انحراف - 1, p->arena);
    اگر (!debug_متن) {
        واپس تہی;
    }

    asdl_اظہاریہ_seq *اقدار = Py_asdl_اظہاریہ_seq_new(2, arena);
    اگر (!اقدار) {
        واپس تہی;
    }
    asdl_seq_سیٹ(اقدار, 0, debug_متن);
    asdl_seq_سیٹ(اقدار, 1, interpolation);
    واپس _PyAST_JoinedStr(اقدار, lineno, ستون_انحراف, debug_انتہا_سطر, debug_انتہا_انحراف, p->arena);
}

اظہاریہ_ty Py_Pegen_formatted_قدر(Parser *p, اظہاریہ_ty عبارت, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                                 نتیجہ_Token_With_Metadata *format, Token *closing_brace, صحیح lineno, صحیح ستون_انحراف,
                                 صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف, PyArena *arena) {
    صحیح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    اظہاریہ_ty formatted_قدر = Py_AST_Formatted_قدر(
        عبارت, conversion_val, format ? (اظہاریہ_ty) format->نتیجہ : تہی,
        lineno, ستون_انحراف, انتہا_lineno,
        انتہا_ستون_انحراف, arena
    );

    اگر (!formatted_قدر || !debug) {
        واپس formatted_قدر;
    }

    /* Find the non whitespace token after the "=" */
    صحیح debug_انتہا_سطر, debug_انتہا_انحراف;
    PyObject *debug_metadata;

    اگر (conversion) {
        debug_انتہا_سطر = ((اظہاریہ_ty) conversion->نتیجہ)->lineno;
        debug_انتہا_انحراف = ((اظہاریہ_ty) conversion->نتیجہ)->ستون_انحراف;
        debug_metadata = conversion->metadata;
    }
    ورنہ اگر (format) {
        debug_انتہا_سطر = ((اظہاریہ_ty) format->نتیجہ)->lineno;
        debug_انتہا_انحراف = ((اظہاریہ_ty) format->نتیجہ)->ستون_انحراف + 1;
        debug_metadata = format->metadata;
    }
    ورنہ {
        debug_انتہا_سطر = انتہا_lineno;
        debug_انتہا_انحراف = انتہا_ستون_انحراف;
        debug_metadata = closing_brace->metadata;
    }
    اظہاریہ_ty debug_متن = _PyAST_Constant(debug_metadata, تہی, lineno, ستون_انحراف + 1, debug_انتہا_سطر,
                                            debug_انتہا_انحراف - 1, p->arena);
    اگر (!debug_متن) {
        واپس تہی;
    }

    asdl_اظہاریہ_seq *اقدار = Py_asdl_اظہاریہ_seq_new(2, arena);
    اگر (!اقدار) {
        واپس تہی;
    }
    asdl_seq_سیٹ(اقدار, 0, debug_متن);
    asdl_seq_سیٹ(اقدار, 1, formatted_قدر);
    واپس _PyAST_JoinedStr(اقدار, lineno, ستون_انحراف, debug_انتہا_سطر, debug_انتہا_انحراف, p->arena);
}

ساکن اظہاریہ_ty
build_concatenated_بائٹیں(Parser *p, asdl_اظہاریہ_seq *لڑیاں, صحیح lineno,
                        صحیح ستون_انحراف, صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *قسم = asdl_seq_GET(لڑیاں, 0)->v.Constant.قسم;

    Py_ssize_t کل = 0;
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        PyObject *بائٹیں = elem->v.Constant.قدر;
        Py_ssize_t part = Py_بائٹیں_GET_سائز(بائٹیں);
        اگر (part > PY_SSIZE_T_MAX - کل) {
            Py_Err_No_حافظہ();
            واپس تہی;
        }
        کل += part;
    }

    Py_بائٹیں_Writer *writer = Py_بائٹیں_Writer_Create(کل);
    اگر (writer == تہی) {
        واپس تہی;
    }
    حرف *برآمد = Py_بائٹیں_Writer_Get_ڈیٹا(writer);

    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        PyObject *بائٹیں = elem->v.Constant.قدر;
        Py_ssize_t part = Py_بائٹیں_GET_سائز(بائٹیں);
        اگر (part > 0) {
            حافظہ_نقل(برآمد, Py_بائٹیں_AS_STRING(بائٹیں), part);
            برآمد += part;
        }
    }

    PyObject *res = Py_بائٹیں_Writer_اختتام(writer);
    اگر (res == تہی) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        واپس تہی;
    }
    واپس _PyAST_Constant(res, قسم, lineno, ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, p->arena);
}

ساکن اظہاریہ_ty
_build_concatenated_unicode(Parser *p, asdl_اظہاریہ_seq *لڑیاں, صحیح lineno,
                        صحیح ستون_انحراف, صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 1);

    اظہاریہ_ty پہلا = asdl_seq_GET(لڑیاں, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *قسم = پہلا->v.Constant.قسم;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    اگر (writer == تہی) {
        واپس تہی;
    }

    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty current_elem = asdl_seq_GET(لڑیاں, i);
        أكد(current_elem->قسم == Constant_قسم);

        اگر (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.قدر)) {
            PyUnicodeWriter_Discard(writer);
            واپس تہی;
        }
    }

    PyObject *final = Py_Unicode_Writer_اختتام(writer);
    اگر (final == تہی) {
        واپس تہی;
    }
    اگر (Py_Arena_جوڑو_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        واپس تہی;
    }
    واپس _PyAST_Constant(final, قسم, lineno, ستون_انحراف,
                           انتہا_lineno, انتہا_ستون_انحراف, arena);
}

ساکن asdl_اظہاریہ_seq *
_build_concatenated_str(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                               صحیح انتہا_ستون_انحراف, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        انتخاب(elem->قسم) {
            حالت Joined_Str_قسم:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.اقدار);
                توڑو;
            حالت Template_Str_قسم:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.اقدار);
                توڑو;
            طےشدہ:
                n_flattened_elements++;
                توڑو;
        }
    }


    asdl_اظہاریہ_seq* flattened = Py_asdl_اظہاریہ_seq_new(n_flattened_elements, p->arena);
    اگر (flattened == تہی) {
        واپس تہی;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    برائے (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        انتخاب(elem->قسم) {
            حالت Joined_Str_قسم:
                برائے (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.اقدار); j++) {
                    اظہاریہ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.اقدار, j);
                    اگر (subvalue == تہی) {
                        واپس تہی;
                    }
                    asdl_seq_سیٹ(flattened, current_pos++, subvalue);
                }
                توڑو;
            حالت Template_Str_قسم:
                برائے (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.اقدار); j++) {
                    اظہاریہ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.اقدار, j);
                    اگر (subvalue == تہی) {
                        واپس تہی;
                    }
                    asdl_seq_سیٹ(flattened, current_pos++, subvalue);
                }
                توڑو;
            طےشدہ:
                asdl_seq_سیٹ(flattened, current_pos++, elem);
                توڑو;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحیح پچھلا_is_constant = 0;
    برائے (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        اگر (elem->قسم == Constant_قسم &&
            PyUnicode_CheckExact(elem->v.Constant.قدر) &&
            Py_Unicode_GET_لمبائی(elem->v.Constant.قدر) == 0)
            جاری;

        اگر (!پچھلا_is_constant || elem->قسم != Constant_قسم) {
            n_elements++;
        }
        پچھلا_is_constant = elem->قسم == Constant_قسم;
    }

    asdl_اظہاریہ_seq* اقدار = Py_asdl_اظہاریہ_seq_new(n_elements, p->arena);
    اگر (اقدار == تہی) {
        واپس تہی;
    }

    /* build folded list */
    current_pos = 0;
    برائے (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        اگر (elem->قسم == Constant_قسم) {
            اگر (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->قسم == Constant_قسم) {
                اظہاریہ_ty پہلا_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *قسم = elem->v.Constant.قسم;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                اگر (writer == تہی) {
                    واپس تہی;
                }
                اظہاریہ_ty آخری_elem = elem;
                Py_ssize_t j;
                برائے (j = i; j < n_flattened_elements; j++) {
                    اظہاریہ_ty current_elem = asdl_seq_GET(flattened, j);
                    اگر (current_elem->قسم == Constant_قسم) {
                        اگر (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.قدر)) {
                            PyUnicodeWriter_Discard(writer);
                            واپس تہی;
                        }
                        آخری_elem = current_elem;
                    } ورنہ {
                        توڑو;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_اختتام(writer);
                اگر (concat_str == تہی) {
                    واپس تہی;
                }
                اگر (Py_Arena_جوڑو_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    واپس تہی;
                }
                elem = _PyAST_Constant(concat_str, قسم, پہلا_elem->lineno,
                                       پہلا_elem->ستون_انحراف,
                                       آخری_elem->انتہا_lineno,
                                       آخری_elem->انتہا_ستون_انحراف, p->arena);
                اگر (elem == تہی) {
                    واپس تہی;
                }
            }

            /* Drop all empty contanst strings */
            اگر (PyUnicode_CheckExact(elem->v.Constant.قدر) &&
                Py_Unicode_GET_لمبائی(elem->v.Constant.قدر) == 0) {
                جاری;
            }
        }

        asdl_seq_سیٹ(اقدار, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    واپس اقدار;
}

ساکن اظہاریہ_ty
_build_concatenated_joined_str(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                               صحیح انتہا_ستون_انحراف, PyArena *arena)
{
    asdl_اظہاریہ_seq *اقدار = _build_concatenated_str(p, لڑیاں, lineno,
        ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
    اگر (!اقدار) {
        واپس تہی;
    }
    واپس _PyAST_JoinedStr(اقدار, lineno, ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, p->arena);
}

اظہاریہ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                               صحیح انتہا_ستون_انحراف, PyArena *arena)
{
    asdl_اظہاریہ_seq *اقدار = _build_concatenated_str(p, لڑیاں, lineno,
        ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
    اگر (!اقدار) {
        واپس تہی;
    }
    واپس _PyAST_TemplateStr(اقدار, lineno, ستون_انحراف, انتہا_lineno,
        انتہا_ستون_انحراف, arena);
}

اظہاریہ_ty
Py_Pegen_concatenate_لڑیاں(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                             صحیح lineno, صحیح ستون_انحراف, صحیح انتہا_lineno,
                             صحیح انتہا_ستون_انحراف, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 0);

    صحیح f_string_ملا = 0;
    صحیح unicode_string_ملا = 0;
    صحیح بائٹیں_ملا = 0;

    Py_ssize_t i = 0;
    برائے (i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        انتخاب(elem->قسم) {
            حالت Constant_قسم:
                اگر (Py_بائٹیں_Check_Exact(elem->v.Constant.قدر)) {
                    بائٹیں_ملا = 1;
                } ورنہ {
                    unicode_string_ملا = 1;
                }
                توڑو;
            حالت Joined_Str_قسم:
                f_string_ملا = 1;
                توڑو;
            حالت Template_Str_قسم:
                // python.gram handles this; we should never get here
                أكد(0);
                توڑو;
            طےشدہ:
                f_string_ملا = 1;
                توڑو;
        }
    }

    // Cannot mix unicode and bytes
    اگر ((unicode_string_ملا || f_string_ملا) && بائٹیں_ملا) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        واپس تہی;
    }

    // If it's only bytes or only unicode string, do a simple concat
    اگر (!f_string_ملا) {
        اگر (len == 1) {
            واپس asdl_seq_GET(لڑیاں, 0);
        }
        ورنہ اگر (بائٹیں_ملا) {
            واپس build_concatenated_بائٹیں(p, لڑیاں, lineno, ستون_انحراف,
                انتہا_lineno, انتہا_ستون_انحراف, arena);
        }
        ورنہ {
            واپس _build_concatenated_unicode(p, لڑیاں, lineno, ستون_انحراف,
                انتہا_lineno, انتہا_ستون_انحراف, arena);
        }
    }

    واپس _build_concatenated_joined_str(p, لڑیاں, lineno,
        ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
}

ساکن صحیح
warn_relative_import_کا_lazy(Parser *p, asdl_seq *dots, اظہاریہ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    اگر (p->call_invalid_قواعد) {
        واپس 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *آخری_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    اگر (
        آخری_dot->انتہا_lineno == module->lineno
        && آخری_dot->انتہا_ستون_انحراف == module->ستون_انحراف
    ) {
        واپس 0;
    }

    صحیح گنتی = Py_Pegen_seq_گنتی_dots(dots);
    حرف *buf = PyMem_RawMalloc(گنتی + 1);
    اگر (buf == تہی) {
        Py_Err_No_حافظہ();
        واپس -1;
    }
    memset(buf, '.', گنتی);
    buf[گنتی] = '\0';

    PyObject *پیغام = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    اگر (پیغام == تہی) {
        واپس -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحیح res = _PyErr_EmitSyntaxWarning(پیغام,
                                       info.filename,
                                       module->lineno,
                                       module->ستون_انحراف + 1,
                                       module->انتہا_lineno,
                                       module->انتہا_ستون_انحراف + 1,
                                       info.module);
    Py_DECREF(پیغام);
    واپس res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, اظہاریہ_ty module_name,
                             asdl_alias_seq *نام, اظہاریہ_ty lazy_token, صحیح lineno,
                             صحیح ستون_انحراف, صحیح انتہا_lineno, صحیح انتہا_ستون_انحراف,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحیح level = Py_Pegen_seq_گنتی_dots(dots);
    اگر (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        اگر (lazy_token) {
            RAISE_SYNTAX_ERROR_معلوم_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            واپس تہی;
        }
        برائے (Py_ssize_t i = 0; i < asdl_seq_LEN(نام); i++) {
            alias_ty alias = asdl_seq_GET(نام, i);
            اگر (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    ورنہ اگر (
        level > 0
        && lazy_token == تہی
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        اگر (warn_relative_import_کا_lazy(p, dots, module_name) < 0) {
            واپس تہی;
        }
    }
    واپس _PyAST_ImportFrom(module, نام, level, lazy_token ? 1 : 0, lineno,
                             ستون_انحراف, انتہا_lineno, انتہا_ستون_انحراف, arena);
}

asdl_stmt_seq*
Py_Pegen_رجسٹر_stmts(Parser *p, asdl_stmt_seq* stmts) {
    اگر (!p->call_invalid_قواعد) {
        واپس stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    اگر (len == 0) {
        واپس stmts;
    }
    stmt_ty آخری_stmt = asdl_seq_GET(stmts, len - 1);
    اگر (p->آخری_stmt_location.lineno > آخری_stmt->lineno) {
        واپس stmts;
    }
    p->آخری_stmt_location.lineno = آخری_stmt->lineno;
    p->آخری_stmt_location.ستون_انحراف = آخری_stmt->ستون_انحراف;
    p->آخری_stmt_location.انتہا_lineno = آخری_stmt->انتہا_lineno;
    p->آخری_stmt_location.انتہا_ستون_انحراف = آخری_stmt->انتہا_ستون_انحراف;
    واپس stmts;
}
