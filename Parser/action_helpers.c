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
    asdl_seq_سیٹ_UNTYPED(seq, 0, a);
    أرجع seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_داخل_کرو_وچ_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    إذا (!seq) {
        أرجع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    asdl_seq_سیٹ_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    أرجع new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_منزل_سرا(Parser *p, asdl_seq *seq, فراغ *a)
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
        asdl_seq_سیٹ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_سیٹ_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    أرجع new_seq;
}

ساكن Py_ssize_t
get_flattened_seq_ناپ(asdl_seq *seqs)
{
    Py_ssize_t ناپ = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        ناپ += asdl_seq_LEN(inner_seq);
    }
    أرجع ناپ;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_ناپ = get_flattened_seq_ناپ(seqs);
    أكد(flattened_seq_ناپ > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_ناپ, p->arena);
    إذا (!flattened_seq) {
        أرجع عدم;
    }

    صحيح flattened_seq_idx = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        لكل (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_سیٹ_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_ناپ);

    أرجع flattened_seq;
}

فراغ *
Py_Pegen_seq_آخری_شے(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أرجع asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
Py_Pegen_seq_پہلا_شے(asdl_seq *seq)
{
    أرجع asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
اظہاریہ_ty
Py_Pegen_join_ناں_with_dot(Parser *p, اظہاریہ_ty پہلا_name, اظہاریہ_ty دوجا_name)
{
    أكد(پہلا_name != عدم && دوجا_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            پہلا_name->v.Name.id, دوجا_name->v.Name.id);
    إذا (!uni) {
        أرجع عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        أرجع عدم;
    }

    أرجع _PyAST_Name(uni, لدو, EXTRA_اظہاریہ(پہلا_name, دوجا_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
Py_Pegen_seq_گنتی_dots(asdl_seq *seq)
{
    صحيح نمبر_دا_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_اظہاریہ = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_اظہاریہ->type) {
            حالة ELLIPSIS:
                نمبر_دا_dots += 3;
                اكسر;
            حالة DOT:
                نمبر_دا_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    أرجع نمبر_دا_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_تارا(Parser *p, صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                        صحيح سرا_خانہ_وتھ, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    إذا (!str) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_alias(str, عدم, lineno, خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_ناں_منزل_ids(Parser *p, asdl_اظہاریہ_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty e = asdl_seq_GET(seq, i);
        asdl_seq_سیٹ(new_seq, i, e->v.Name.id);
    }
    أرجع new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_اظہاریہ_Pair *
Py_Pegen_cmpop_اظہاریہ_pair(Parser *p, cmpop_ty cmpop, اظہاریہ_ty اظہاریہ)
{
    أكد(اظہاریہ != عدم);
    Cmpop_اظہاریہ_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_اظہاریہ_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->cmpop = cmpop;
    a->اظہاریہ = اظہاریہ;
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
        Cmpop_اظہاریہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->cmpop);
    }
    أرجع new_seq;
}

asdl_اظہاریہ_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_اظہاریہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->اظہاریہ);
    }
    أرجع new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_اظہاریہ_seq *
سیٹ_seq_context(Parser *p, asdl_اظہاریہ_seq *seq, اظہاریہ_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إذا (len == 0) {
        أرجع عدم;
    }

    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty e = asdl_seq_GET(seq, i);
        اظہاریہ_ty new_e = Py_Pegen_سیٹ_اظہاریہ_context(p, e, ctx);
        إذا (!new_e) {
            أرجع عدم;
        }
        asdl_seq_سیٹ(new_seq, i, new_e);
    }
    أرجع new_seq;
}

ساكن اظہاریہ_ty
سیٹ_name_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    أرجع _PyAST_Name(e->v.Name.id, ctx, EXTRA_اظہاریہ(e, e));
}

ساكن اظہاریہ_ty
سیٹ_tuple_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    asdl_اظہاریہ_seq *seq = سیٹ_seq_context(p, e->v.Tuple.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_Tuple(seq, ctx, EXTRA_اظہاریہ(e, e));
}

ساكن اظہاریہ_ty
سیٹ_list_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    asdl_اظہاریہ_seq *seq = سیٹ_seq_context(p, e->v.List.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_List(seq, ctx, EXTRA_اظہاریہ(e, e));
}

ساكن اظہاریہ_ty
سیٹ_subscript_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    أرجع _PyAST_Subscript(e->v.Subscript.قدر, e->v.Subscript.ٹکڑا,
                            ctx, EXTRA_اظہاریہ(e, e));
}

ساكن اظہاریہ_ty
سیٹ_attribute_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    أرجع _PyAST_Attribute(e->v.Attribute.قدر, e->v.Attribute.attr,
                            ctx, EXTRA_اظہاریہ(e, e));
}

ساكن اظہاریہ_ty
سیٹ_starred_context(Parser *p, اظہاریہ_ty e, اظہاریہ_context_ty ctx)
{
    اظہاریہ_ty inner = Py_Pegen_سیٹ_اظہاریہ_context(p, e->v.Starred.قدر, ctx);
    إذا (!inner) {
        أرجع عدم;
    }
    أرجع _PyAST_Starred(inner, ctx, EXTRA_اظہاریہ(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
اظہاریہ_ty
Py_Pegen_سیٹ_اظہاریہ_context(Parser *p, اظہاریہ_ty اظہاریہ, اظہاریہ_context_ty ctx)
{
    أكد(اظہاریہ != عدم);

    اظہاریہ_ty new = عدم;
    بدل (اظہاریہ->قسم) {
        حالة Name_قسم:
            new = سیٹ_name_context(p, اظہاریہ, ctx);
            اكسر;
        حالة Tuple_قسم:
            new = سیٹ_tuple_context(p, اظہاریہ, ctx);
            اكسر;
        حالة List_قسم:
            new = سیٹ_list_context(p, اظہاریہ, ctx);
            اكسر;
        حالة Subscript_قسم:
            new = سیٹ_subscript_context(p, اظہاریہ, ctx);
            اكسر;
        حالة Attribute_قسم:
            new = سیٹ_attribute_context(p, اظہاریہ, ctx);
            اكسر;
        حالة Starred_قسم:
            new = سیٹ_starred_context(p, اظہاریہ, ctx);
            اكسر;
        افتراضي:
            new = اظہاریہ;
    }
    أرجع new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
اظہاریہ_ty
Py_Pegen_make_نشانہ(Parser *p, اظہاریہ_ty اظہاریہ, TARGETS_TYPE targets_type)
{
    أكد(اظہاریہ != عدم);
    أكد(targets_type != FOR_TARGETS);
    إذا (Py_Pegen_get_invalid_نشانہ(اظہاریہ, targets_type) != عدم) {
        أرجع عدم;
    }
    أرجع Py_Pegen_سیٹ_اظہاریہ_context(
        p, اظہاریہ, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
کلید_قدر_Pair *
Py_Pegen_کلید_قدر_pair(Parser *p, اظہاریہ_ty کلید, اظہاریہ_ty قدر)
{
    کلید_قدر_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_قدر_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلید = کلید;
    a->قدر = قدر;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_کلیدیں(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_قدر_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->کلید);
    }
    أرجع new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_اقدار(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_قدر_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->قدر);
    }
    أرجع new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
کلید_نمونہ_Pair *
Py_Pegen_کلید_نمونہ_pair(Parser *p, اظہاریہ_ty کلید, نمونہ_ty نمونہ)
{
    کلید_نمونہ_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_نمونہ_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلید = کلید;
    a->نمونہ = نمونہ;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_اظہاریہ_seq *
Py_Pegen_get_نمونہ_کلیدیں(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_نمونہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->کلید);
    }
    أرجع new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_نمونہ_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_نمونہ_seq *new_seq = Py_asdl_نمونہ_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_نمونہ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سیٹ(new_seq, i, pair->نمونہ);
    }
    أرجع new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, اظہاریہ_ty قدر, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    إذا (!a) {
        أرجع عدم;
    }
    a->arg = Py_Pegen_جوڑو_type_comment_منزل_arg(p, arg, tc);
    إذا (!a->arg) {
        أرجع عدم;
    }
    a->قدر = قدر;
    أرجع a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_ناں, asdl_seq *ناں_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    إذا (!a) {
        أرجع عدم;
    }
    a->plain_ناں = plain_ناں;
    a->ناں_with_defaults = ناں_with_defaults;
    أرجع a;
}

/* Constructs a StarEtc */
تارا_Etc *
Py_Pegen_تارا_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    تارا_Etc *a = _PyArena_Malloc(p->arena, حجم(تارا_Etc));
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
    Py_ssize_t پہلا_len = asdl_seq_LEN(a);
    Py_ssize_t دوجا_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(پہلا_len + دوجا_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < پہلا_len; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < دوجا_len; i++) {
        asdl_seq_سیٹ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    أرجع new_seq;
}

ساكن asdl_arg_seq*
get_ناں(Parser *p, asdl_seq *ناں_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ناں_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ناں_with_defaults, i);
        asdl_seq_سیٹ(seq, i, pair->arg);
    }
    أرجع seq;
}

ساكن asdl_اظہاریہ_seq *
_get_defaults(Parser *p, asdl_seq *ناں_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(ناں_with_defaults);
    asdl_اظہاریہ_seq *seq = Py_asdl_اظہاریہ_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(ناں_with_defaults, i);
        asdl_seq_سیٹ(seq, i, pair->قدر);
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
        asdl_arg_seq *slash_with_default_ناں =
                get_ناں(p, slash_with_default->ناں_with_defaults);
        إذا (!slash_with_default_ناں) {
            أرجع -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_ناں,
                (asdl_seq*)slash_with_default_ناں);
    }
    وإلا {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    أرجع *posonlyargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_ناں,
              asdl_seq *ناں_with_default,
              asdl_arg_seq **posargs) {

    إذا (ناں_with_default != عدم) {
        إذا (plain_ناں != عدم) {
            asdl_arg_seq *ناں_with_default_ناں = get_ناں(p, ناں_with_default);
            إذا (!ناں_with_default_ناں) {
                أرجع -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_ناں, (asdl_seq*)ناں_with_default_ناں);
        }
        وإلا {
            *posargs = get_ناں(p, ناں_with_default);
        }
    }
    وإلا {
        إذا (plain_ناں != عدم) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_ناں;
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
                  asdl_seq *ناں_with_default,
                  asdl_اظہاریہ_seq **posdefaults) {
    إذا (slash_with_default != عدم && ناں_with_default != عدم) {
        asdl_اظہاریہ_seq *slash_with_default_اقدار =
                _get_defaults(p, slash_with_default->ناں_with_defaults);
        إذا (!slash_with_default_اقدار) {
            أرجع -1;
        }
        asdl_اظہاریہ_seq *ناں_with_default_اقدار = _get_defaults(p, ناں_with_default);
        إذا (!ناں_with_default_اقدار) {
            أرجع -1;
        }
        *posdefaults = (asdl_اظہاریہ_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_اقدار,
                (asdl_seq*)ناں_with_default_اقدار);
    }
    وإلا إذا (slash_with_default == عدم && ناں_with_default != عدم) {
        *posdefaults = _get_defaults(p, ناں_with_default);
    }
    وإلا إذا (slash_with_default != عدم && ناں_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->ناں_with_defaults);
    }
    وإلا {
        *posdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    }
    أرجع *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, تارا_Etc *تارا_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_اظہاریہ_seq **kwdefaults) {
    إذا (تارا_etc != عدم && تارا_etc->kwonlyargs != عدم) {
        *kwonlyargs = get_ناں(p, تارا_etc->kwonlyargs);
    }
    وإلا {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    إذا (*kwonlyargs == عدم) {
        أرجع -1;
    }

    إذا (تارا_etc != عدم && تارا_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, تارا_etc->kwonlyargs);
    }
    وإلا {
        *kwdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    }

    إذا (*kwdefaults == عدم) {
        أرجع -1;
    }

    أرجع 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_ناں,
                        asdl_seq *ناں_with_default, تارا_Etc *تارا_etc)
{
    asdl_arg_seq *posonlyargs;
    إذا (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        أرجع عدم;
    }

    asdl_arg_seq *posargs;
    إذا (_make_posargs(p, plain_ناں, ناں_with_default, &posargs) == -1) {
        أرجع عدم;
    }

    asdl_اظہاریہ_seq *posdefaults;
    إذا (_make_posdefaults(p,slash_with_default, ناں_with_default, &posdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty vararg = عدم;
    إذا (تارا_etc != عدم && تارا_etc->vararg != عدم) {
        vararg = تارا_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_اظہاریہ_seq *kwdefaults;
    إذا (_make_kwargs(p, تارا_etc, &kwonlyargs, &kwdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty kwarg = عدم;
    إذا (تارا_etc != عدم && تارا_etc->kwarg != عدم) {
        kwarg = تارا_etc->kwarg;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_خالی_تھاواں_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posonlyargs) {
        أرجع عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posargs) {
        أرجع عدم;
    }
    asdl_اظہاریہ_seq *posdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    إذا (!posdefaults) {
        أرجع عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!kwonlyargs) {
        أرجع عدم;
    }
    asdl_اظہاریہ_seq *kwdefaults = Py_asdl_اظہاریہ_seq_new(0, p->arena);
    إذا (!kwdefaults) {
        أرجع عدم;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty قسم)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    إذا (!a) {
        أرجع عدم;
    }
    a->قسم = قسم;
    أرجع a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_اظہاریہ_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    إذا (function_def->قسم == Async_Function_Def_قسم) {
        أرجع _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.دلیلاں,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->خانہ_وتھ,
            function_def->سرا_lineno, function_def->سرا_خانہ_وتھ, p->arena);
    }

    أرجع _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.دلیلاں,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->خانہ_وتھ,
        function_def->سرا_lineno, function_def->سرا_خانہ_وتھ, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_اظہاریہ_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != عدم);
    أرجع _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->خانہ_وتھ, class_def->سرا_lineno,
        class_def->سرا_خانہ_وتھ, p->arena);
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
seq_نمبر_دا_starred_exprs(asdl_seq *seq)
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
asdl_اظہاریہ_seq *
Py_Pegen_seq_کڈھو_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = seq_نمبر_دا_starred_exprs(kwargs);
    إذا (new_len == 0) {
        أرجع عدم;
    }
    asdl_اظہاریہ_seq *new_seq = Py_asdl_اظہاریہ_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (!k->is_keyword) {
            asdl_seq_سیٹ(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_نمبر_دا_starred_exprs(kwargs);
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
            asdl_seq_سیٹ(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

اظہاریہ_ty
_PyPegen_ensure_imaginary(Parser *p, اظہاریہ_ty قوت_نما)
{
    إذا (قوت_نما->قسم != Constant_قسم || !PyComplex_CheckExact(قوت_نما->v.Constant.قدر)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(قوت_نما, "imaginary number required in complex literal");
        أرجع عدم;
    }
    أرجع قوت_نما;
}

اظہاریہ_ty
_PyPegen_ensure_real(Parser *p, اظہاریہ_ty قوت_نما)
{
    إذا (قوت_نما->قسم != Constant_قسم || PyComplex_CheckExact(قوت_نما->v.Constant.قدر)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(قوت_نما, "real number required in complex literal");
        أرجع عدم;
    }
    أرجع قوت_نما;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t عدد = p->type_ignore_comments.عدد_اشیاء;
    إذا (عدد > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(عدد, p->arena);
        إذا (type_ignores == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < عدد; i++) {
            PyObject *ٹیگ = _PyPegen_new_type_comment(p, p->type_ignore_comments.اشیاء[i].comment);
            إذا (ٹیگ == عدم) {
                أرجع عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.اشیاء[i].lineno,
                                                  ٹیگ, p->arena);
            إذا (ti == عدم) {
                أرجع عدم;
            }
            asdl_seq_سیٹ(type_ignores, i, ti);
        }
    }
    أرجع _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, لڑی_لمبائی(s), عدم);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع res;
}

arg_ty
Py_Pegen_جوڑو_type_comment_منزل_arg(Parser *p, arg_ty a, Token *tc)
{
    إذا (tc == عدم) {
        أرجع a;
    }
    ثابت حرف *بائٹاں = Py_بائٹاں_As_String(tc->بائٹاں);
    إذا (بائٹاں == عدم) {
        أرجع عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بائٹاں);
    إذا (tco == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->خانہ_وتھ, a->سرا_lineno, a->سرا_خانہ_وتھ,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحيح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->بائٹاں != عدم);
    أكد(t->type == NOTEQUAL);

    ثابت حرف* tok_str = Py_بائٹاں_AS_STRING(t->بائٹاں);
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
_PyPegen_check_legacy_stmt(Parser *p, اظہاریہ_ty name) {
    إذا (name->قسم != Name_قسم) {
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
_PyPegen_raise_error_for_missing_comma(Parser *p, اظہاریہ_ty a, اظہاریہ_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    إذا (_PyPegen_check_legacy_stmt(p, a)) {
        أرجع عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    إذا (p->ٹوکن[p->mark - 1]->level == 0) {
        أرجع عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    إذا (a->سرا_lineno > a->lineno) {
        أرجع RAISE_ERROR_معلوم_LOCATION(
            p, PyExc_SyntaxError, a->سرا_lineno, a->خانہ_وتھ,
            a->سرا_lineno, a->سرا_خانہ_وتھ,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    أرجع RAISE_ERROR_معلوم_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->خانہ_وتھ,
        b->سرا_lineno, b->سرا_خانہ_وتھ,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساكن نتیجہ_Token_With_Metadata *
نتیجہ_token_with_metadata(Parser *p, فراغ *نتیجہ, PyObject *metadata)
{
    نتیجہ_Token_With_Metadata *res = _PyArena_Malloc(p->arena, حجم(نتیجہ_Token_With_Metadata));
    إذا (res == عدم) {
        أرجع عدم;
    }
    res->metadata = metadata;
    res->نتیجہ = نتیجہ;
    أرجع res;
}

ساكن حرف
formatted_string_اگیتر(ثابت Parser *p)
{
    صحيح nested = 0;
    لكل (صحيح i = p->mark - 1; i >= 0; i--) {
        صحيح type = p->ٹوکن[i]->type;
        إذا (type == FSTRING_سرا || type == TSTRING_سرا) {
            nested++;
        }
        وإلا إذا (type == FSTRING_شروع || type == TSTRING_شروع) {
            إذا (nested == 0) {
                أرجع type == TSTRING_شروع ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

نتیجہ_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, اظہاریہ_ty conv)
{
    إذا (conv_token->lineno != conv->lineno || conv_token->سرا_خانہ_وتھ != conv->خانہ_وتھ) {
        أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_اگیتر(p)
        );
    }

    Py_UCS4 پہلا = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    إذا (Py_Unicode_GET_لمائی(conv->v.Name.id) > 1 ||
            !(پہلا == 's' || پہلا == 'r' || پہلا == 'a')) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_اگیتر(p),
                                            conv->v.Name.id);
        أرجع عدم;
    }

    أرجع نتیجہ_token_with_metadata(p, conv, conv_token->metadata);
}

نتیجہ_Token_With_Metadata *
Py_Pegen_تیاری_بھریا_format_spec(Parser *p, Token *colon, asdl_اظہاریہ_seq *spec, صحيح lineno, صحيح خانہ_وتھ,
                                صحيح سرا_lineno, صحيح سرا_خانہ_وتھ, PyArena *arena)
{
    إذا (!spec) {
        أرجع عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_اشیاء = asdl_seq_LEN(spec);
    Py_ssize_t non_خالی_تھاواں_گنتی = 0;
    لكل (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(spec, i);
        non_خالی_تھاواں_گنتی += !(شے->قسم == Constant_قسم &&
                             PyUnicode_CheckExact(شے->v.Constant.قدر) &&
                             Py_Unicode_GET_لمائی(شے->v.Constant.قدر) == 0);
    }
    إذا (non_خالی_تھاواں_گنتی != n_اشیاء) {
        asdl_اظہاریہ_seq *resized_spec =
            Py_asdl_اظہاریہ_seq_new(non_خالی_تھاواں_گنتی, p->arena);
        إذا (resized_spec == عدم) {
            أرجع عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_اشیاء; i++) {
            اظہاریہ_ty شے = asdl_seq_GET(spec, i);
            إذا (شے->قسم == Constant_قسم &&
                PyUnicode_CheckExact(شے->v.Constant.قدر) &&
                Py_Unicode_GET_لمائی(شے->v.Constant.قدر) == 0) {
                استمر;
            }
            asdl_seq_سیٹ(resized_spec, j++, شے);
        }
        أكد(j == non_خالی_تھاواں_گنتی);
        spec = resized_spec;
    }
    اظہاریہ_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    إذا (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->قسم == Constant_قسم)) {
        res = _PyAST_JoinedStr(spec, lineno, خانہ_وتھ, سرا_lineno,
                                    سرا_خانہ_وتھ, p->arena);
    } وإلا {
        res = Py_Pegen_concatenate_لڑیاں(p, spec,
                             lineno, خانہ_وتھ, سرا_lineno,
                             سرا_خانہ_وتھ, arena);
    }
    إذا (!res) {
        أرجع عدم;
    }
    أرجع نتیجہ_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_اظہاریہ_name(اظہاریہ_ty e)
{
    أكد(e != عدم);
    بدل (e->قسم) {
        حالة Attribute_قسم:
            أرجع "attribute";
        حالة Subscript_قسم:
            أرجع "subscript";
        حالة Starred_قسم:
            أرجع "starred";
        حالة Name_قسم:
            أرجع "name";
        حالة List_قسم:
            أرجع "list";
        حالة Tuple_قسم:
            أرجع "tuple";
        حالة Lambda_قسم:
            أرجع "lambda";
        حالة Call_قسم:
            أرجع "function call";
        حالة Bool_Op_قسم:
        حالة Bin_Op_قسم:
        حالة Unary_Op_قسم:
            أرجع "expression";
        حالة Generator_Exp_قسم:
            أرجع "generator expression";
        حالة Yield_قسم:
        حالة Yield_From_قسم:
            أرجع "yield expression";
        حالة Await_قسم:
            أرجع "await expression";
        حالة List_Comp_قسم:
            أرجع "list comprehension";
        حالة سیٹ_Comp_قسم:
            أرجع "set comprehension";
        حالة Dict_Comp_قسم:
            أرجع "dict comprehension";
        حالة Dict_قسم:
            أرجع "dict literal";
        حالة سیٹ_قسم:
            أرجع "set display";
        حالة Joined_Str_قسم:
        حالة Formatted_قدر_قسم:
            أرجع "f-string expression";
        حالة Template_Str_قسم:
        حالة Interpolation_قسم:
            أرجع "t-string expression";
        حالة Constant_قسم: {
            PyObject *قدر = e->v.Constant.قدر;
            إذا (قدر == Py_None) {
                أرجع "None";
            }
            إذا (قدر == Py_False) {
                أرجع "False";
            }
            إذا (قدر == Py_True) {
                أرجع "True";
            }
            إذا (قدر == Py_Ellipsis) {
                أرجع "ellipsis";
            }
            أرجع "literal";
        }
        حالة Compare_قسم:
            أرجع "comparison";
        حالة If_Exp_قسم:
            أرجع "conditional expression";
        حالة Named_اظہاریہ_قسم:
            أرجع "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->قسم, e->lineno);
            أرجع عدم;
    }
}

اظہاریہ_ty
Py_Pegen_get_آخری_comprehension_شے(comprehension_ty comprehension) {
    إذا (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        أرجع comprehension->iter;
    }
    أرجع Py_Pegen_آخری_شے(comprehension->ifs, اظہاریہ_ty);
}

اظہاریہ_ty _PyPegen_collect_call_seqs(Parser *p, asdl_اظہاریہ_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                     صحيح سرا_خانہ_وتھ, PyArena *arena) {
    Py_ssize_t دلیلاں_len = asdl_seq_LEN(a);
    Py_ssize_t کل_len = دلیلاں_len;

    إذا (b == عدم) {
        أرجع _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, خانہ_وتھ,
                        سرا_lineno, سرا_خانہ_وتھ, arena);

    }

    asdl_اظہاریہ_seq *starreds = Py_Pegen_seq_کڈھو_starred_exprs(p, b);
    إذا (!starreds && PyErr_Occurred()) {
        أرجع عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    إذا (!keywords && PyErr_Occurred()) {
        أرجع عدم;
    }

    إذا (starreds) {
        کل_len += asdl_seq_LEN(starreds);
    }

    asdl_اظہاریہ_seq *دلیلاں = Py_asdl_اظہاریہ_seq_new(کل_len, arena);
    إذا (دلیلاں == عدم) {
        أرجع عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < دلیلاں_len; i++) {
        asdl_seq_سیٹ(دلیلاں, i, asdl_seq_GET(a, i));
    }
    لكل (; i < کل_len; i++) {
        asdl_seq_سیٹ(دلیلاں, i, asdl_seq_GET(starreds, i - دلیلاں_len));
    }

    أرجع _PyAST_Call(_PyPegen_dummy_name(p), دلیلاں, keywords, lineno,
                       خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
}

// AST Error reporting helpers

اظہاریہ_ty
Py_Pegen_get_invalid_نشانہ(اظہاریہ_ty e, TARGETS_TYPE targets_type)
{
    إذا (e == عدم) {
        أرجع عدم;
    }

    إذا (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        إذا (targets_type == SINGLE_TARGETS && e->قسم == Name_قسم) {
            أرجع عدم;
        }
        أرجع e->قسم == Attribute_قسم || e->قسم == Subscript_قسم ? عدم : e;
    }

#تعریف_کرو ویکھو_CONTAINER(CONTAINER, TYPE) افعل { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            اظہاریہ_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            اظہاریہ_ty بچہ = Py_Pegen_get_invalid_نشانہ(other, targets_type);\
            إذا (بچہ != عدم) {\
                أرجع بچہ;\
            }\
        }\
    } طالما (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->قسم) {
        حالة List_قسم:
            ویکھو_CONTAINER(e, List);
            أرجع عدم;
        حالة Tuple_قسم:
            ویکھو_CONTAINER(e, Tuple);
            أرجع عدم;
        حالة Starred_قسم:
            إذا (targets_type == DEL_TARGETS) {
                أرجع e;
            }
            أرجع Py_Pegen_get_invalid_نشانہ(e->v.Starred.قدر, targets_type);
        حالة Compare_قسم:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            إذا (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                إذا (cmpop == وچ) {
                    أرجع Py_Pegen_get_invalid_نشانہ(e->v.Compare.بایاں, targets_type);
                }
                أرجع عدم;
            }
            أرجع e;
        حالة Name_قسم:
        حالة Subscript_قسم:
        حالة Attribute_قسم:
            أرجع عدم;
        افتراضي:
            أرجع e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, اظہاریہ_ty e) {
    صحيح kwarg_unpacking = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        إذا (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *پیغام = عدم;
    إذا (kwarg_unpacking) {
        پیغام = "positional argument follows keyword argument unpacking";
    } وإلا {
        پیغام = "positional argument follows keyword argument";
    }

    أرجع RAISE_SYNTAX_ERROR(پیغام);
}

فراغ *
Py_Pegen_nonparen_genexp_وچ_call(Parser *p, اظہاریہ_ty دلیلاں, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(دلیلاں->v.Call.دلیلاں);
    إذا (len <= 1) {
        أرجع عدم;
    }

    comprehension_ty آخری_comprehension = Py_Pegen_آخری_شے(comprehensions, comprehension_ty);

    أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
        (اظہاریہ_ty) asdl_seq_GET(دلیلاں->v.Call.دلیلاں, len - 1),
        Py_Pegen_get_آخری_comprehension_شے(آخری_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن اظہاریہ_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, اظہاریہ_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.قدر));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.قدر);
    إذا (bstr == عدم) {
        أرجع عدم;
    }

    ناپ_t len;
    إذا (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } وإلا {
        len = لڑی_لمبائی(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == عدم;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    إذا (str == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, constant->lineno, constant->خانہ_وتھ,
                           constant->سرا_lineno, constant->سرا_خانہ_وتھ,
                           p->arena);
}

ساكن asdl_اظہاریہ_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_اشیاء = asdl_seq_LEN(raw_expressions);
    Py_ssize_t کل_اشیاء = n_اشیاء;
    لكل (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(raw_expressions, i);
        إذا (شے->قسم == Joined_Str_قسم) {
            کل_اشیاء += asdl_seq_LEN(شے->v.JoinedStr.اقدار) - 1;
        }
    }

    ثابت حرف* quote_str = Py_بائٹاں_As_String(a->بائٹاں);
    إذا (quote_str == عدم) {
        أرجع عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_اظہاریہ_seq *seq = Py_asdl_اظہاریہ_seq_new(کل_اشیاء, p->arena);
    إذا (seq == عدم) {
        أرجع عدم;
    }

    Py_ssize_t اشاریہ = 0;
    لكل (Py_ssize_t i = 0; i < n_اشیاء; i++) {
        اظہاریہ_ty شے = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        إذا (شے->قسم == Joined_Str_قسم) {
            asdl_اظہاریہ_seq *اقدار = شے->v.JoinedStr.اقدار;
            إذا (asdl_seq_LEN(اقدار) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             شے->lineno);
                أرجع عدم;
            }

            اظہاریہ_ty پہلا = asdl_seq_GET(اقدار, 0);
            أكد(پہلا->قسم == Constant_قسم);
            asdl_seq_سیٹ(seq, اشاریہ++, پہلا);

            اظہاریہ_ty دوجا = asdl_seq_GET(اقدار, 1);
            أكد((is_tstring &&
                    دوجا->قسم == Interpolation_قسم) ||
                   دوجا->قسم == Formatted_قدر_قسم);
            asdl_seq_سیٹ(seq, اشاریہ++, دوجا);

            استمر;
        }

        إذا (شے->قسم == Constant_قسم) {
            شے = _PyPegen_decode_fstring_part(p, is_raw, شے, b);
            إذا (شے == عدم) {
                أرجع عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            إذا (PyUnicode_CheckExact(شے->v.Constant.قدر)
                && Py_Unicode_GET_لمائی(شے->v.Constant.قدر) == 0) {
                استمر;
            }
        }
        asdl_seq_سیٹ(seq, اشاریہ++, شے);
    }

    asdl_اظہاریہ_seq *resized_exprs;
    إذا (اشاریہ != کل_اشیاء) {
        resized_exprs = Py_asdl_اظہاریہ_seq_new(اشاریہ, p->arena);
        إذا (resized_exprs == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < اشاریہ; i++) {
            asdl_seq_سیٹ(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وإلا {
        resized_exprs = seq;
    }
    أرجع resized_exprs;
}

اظہاریہ_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_اظہاریہ_seq *raw_expressions, Token *b) {

    asdl_اظہاریہ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(resized_exprs, a->lineno, a->خانہ_وتھ,
                              b->سرا_lineno, b->سرا_خانہ_وتھ,
                              p->arena);
}

اظہاریہ_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_اظہاریہ_seq* raw_expressions, Token*b) {

    asdl_اظہاریہ_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(resized_exprs, a->lineno, a->خانہ_وتھ,
                            b->سرا_lineno, b->سرا_خانہ_وتھ,
                            p->arena);
}

اظہاریہ_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بائٹاں == عدم) {
        أرجع Py_Pegen_raise_incomplete_داخلہ(p);
    }
    Py_ssize_t bsize = Py_بائٹاں_GET_ناپ(tok->بائٹاں);
    ثابت حرف *bstr = Py_بائٹاں_AS_STRING(tok->بائٹاں);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->خانہ_وتھ,
                           tok->سرا_lineno, tok->سرا_خانہ_وتھ,
                           p->arena);
}

اظہاریہ_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بائٹاں == عدم) {
        أرجع Py_Pegen_raise_incomplete_داخلہ(p);
    }
    ثابت حرف *bstr = Py_بائٹاں_AS_STRING(tok->بائٹاں);
    PyObject* str = PyUnicode_FromString(bstr);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->خانہ_وتھ,
                           tok->سرا_lineno, tok->سرا_خانہ_وتھ,
                           p->arena);
}

اظہاریہ_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    إذا (tok->بائٹاں == عدم) {
        أرجع Py_Pegen_raise_incomplete_داخلہ(p);
    }
    ثابت حرف *the_str = Py_بائٹاں_AS_STRING(tok->بائٹاں);
    PyObject *s = _PyPegen_parse_string(p, tok);
    إذا (s == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        أرجع عدم;
    }
    PyObject *قسم = عدم;
    إذا (the_str && the_str[0] == 'u') {
        قسم = _PyPegen_new_identifier(p, "u");
        إذا (قسم == عدم) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_Constant(s, قسم, tok->lineno, tok->خانہ_وتھ, tok->سرا_lineno, tok->سرا_خانہ_وتھ, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                              نتیجہ_Token_With_Metadata *format)
{
    إذا (conversion != عدم) {
        اظہاریہ_ty conversion_اظہاریہ = (اظہاریہ_ty) conversion->نتیجہ;
        أكد(conversion_اظہاریہ->قسم == Name_قسم);
        Py_UCS4 پہلا = PyUnicode_READ_CHAR(conversion_اظہاریہ->v.Name.id, 0);
        أرجع Py_محفوظ_DOWNCAST(پہلا, Py_UCS4, صحيح);
    }
    وإلا إذا (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        أرجع (صحيح)'r';
    }
    أرجع -1;
}

ساكن PyObject *
strip_interpolation_debug_اظہاریہ(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_لمائی(exprstr);

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

اظہاریہ_ty _PyPegen_interpolation(Parser *p, اظہاریہ_ty عبارت, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                                 نتیجہ_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح خانہ_وتھ,
                                 صحيح سرا_lineno, صحيح سرا_خانہ_وتھ, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_سرا_سطر, debug_سرا_وتھ;
    PyObject *debug_metadata;
    constant exprstr;

    إذا (conversion) {
        debug_سرا_سطر = ((اظہاریہ_ty) conversion->نتیجہ)->lineno;
        debug_سرا_وتھ = ((اظہاریہ_ty) conversion->نتیجہ)->خانہ_وتھ;
        debug_metadata = exprstr = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_سرا_سطر = ((اظہاریہ_ty) format->نتیجہ)->lineno;
        debug_سرا_وتھ = ((اظہاریہ_ty) format->نتیجہ)->خانہ_وتھ + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وإلا {
        debug_سرا_سطر = سرا_lineno;
        debug_سرا_وتھ = سرا_خانہ_وتھ;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_اظہاریہ(exprstr)
        : Py_NewRef(exprstr);
    إذا (!final_exprstr || Py_Arena_جوڑو_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        أرجع عدم;
    }

    اظہاریہ_ty interpolation = _PyAST_Interpolation(
        عبارت, final_exprstr, conversion_val, format ? (اظہاریہ_ty) format->نتیجہ : عدم,
        lineno, خانہ_وتھ, سرا_lineno,
        سرا_خانہ_وتھ, arena
    );

    إذا (!interpolation || !debug) {
        أرجع interpolation;
    }

    اظہاریہ_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, خانہ_وتھ + 1, debug_سرا_سطر,
                                            debug_سرا_وتھ - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_اظہاریہ_seq *اقدار = Py_asdl_اظہاریہ_seq_new(2, arena);
    إذا (!اقدار) {
        أرجع عدم;
    }
    asdl_seq_سیٹ(اقدار, 0, debug_متن);
    asdl_seq_سیٹ(اقدار, 1, interpolation);
    أرجع _PyAST_JoinedStr(اقدار, lineno, خانہ_وتھ, debug_سرا_سطر, debug_سرا_وتھ, p->arena);
}

اظہاریہ_ty Py_Pegen_formatted_قدر(Parser *p, اظہاریہ_ty عبارت, Token *debug, نتیجہ_Token_With_Metadata *conversion,
                                 نتیجہ_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح خانہ_وتھ,
                                 صحيح سرا_lineno, صحيح سرا_خانہ_وتھ, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    اظہاریہ_ty formatted_قدر = Py_AST_Formatted_قدر(
        عبارت, conversion_val, format ? (اظہاریہ_ty) format->نتیجہ : عدم,
        lineno, خانہ_وتھ, سرا_lineno,
        سرا_خانہ_وتھ, arena
    );

    إذا (!formatted_قدر || !debug) {
        أرجع formatted_قدر;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_سرا_سطر, debug_سرا_وتھ;
    PyObject *debug_metadata;

    إذا (conversion) {
        debug_سرا_سطر = ((اظہاریہ_ty) conversion->نتیجہ)->lineno;
        debug_سرا_وتھ = ((اظہاریہ_ty) conversion->نتیجہ)->خانہ_وتھ;
        debug_metadata = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_سرا_سطر = ((اظہاریہ_ty) format->نتیجہ)->lineno;
        debug_سرا_وتھ = ((اظہاریہ_ty) format->نتیجہ)->خانہ_وتھ + 1;
        debug_metadata = format->metadata;
    }
    وإلا {
        debug_سرا_سطر = سرا_lineno;
        debug_سرا_وتھ = سرا_خانہ_وتھ;
        debug_metadata = closing_brace->metadata;
    }
    اظہاریہ_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, خانہ_وتھ + 1, debug_سرا_سطر,
                                            debug_سرا_وتھ - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_اظہاریہ_seq *اقدار = Py_asdl_اظہاریہ_seq_new(2, arena);
    إذا (!اقدار) {
        أرجع عدم;
    }
    asdl_seq_سیٹ(اقدار, 0, debug_متن);
    asdl_seq_سیٹ(اقدار, 1, formatted_قدر);
    أرجع _PyAST_JoinedStr(اقدار, lineno, خانہ_وتھ, debug_سرا_سطر, debug_سرا_وتھ, p->arena);
}

ساكن اظہاریہ_ty
build_concatenated_بائٹاں(Parser *p, asdl_اظہاریہ_seq *لڑیاں, صحيح lineno,
                        صحيح خانہ_وتھ, صحيح سرا_lineno, صحيح سرا_خانہ_وتھ,
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
    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        PyObject *بائٹاں = elem->v.Constant.قدر;
        Py_ssize_t part = Py_بائٹاں_GET_ناپ(بائٹاں);
        إذا (part > PY_SSIZE_T_MAX - کل) {
            Py_Err_No_حافظہ();
            أرجع عدم;
        }
        کل += part;
    }

    Py_بائٹاں_Writer *writer = Py_بائٹاں_Writer_Create(کل);
    إذا (writer == عدم) {
        أرجع عدم;
    }
    حرف *باہرلا = Py_بائٹاں_Writer_Get_انکڑے(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        PyObject *بائٹاں = elem->v.Constant.قدر;
        Py_ssize_t part = Py_بائٹاں_GET_ناپ(بائٹاں);
        إذا (part > 0) {
            حافظہ_نقل(باہرلا, Py_بائٹاں_AS_STRING(بائٹاں), part);
            باہرلا += part;
        }
    }

    PyObject *res = Py_بائٹاں_Writer_انت(writer);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(res, قسم, lineno, خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, p->arena);
}

ساكن اظہاریہ_ty
_build_concatenated_unicode(Parser *p, asdl_اظہاریہ_seq *لڑیاں, صحيح lineno,
                        صحيح خانہ_وتھ, صحيح سرا_lineno, صحيح سرا_خانہ_وتھ,
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
    إذا (writer == عدم) {
        أرجع عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty current_elem = asdl_seq_GET(لڑیاں, i);
        أكد(current_elem->قسم == Constant_قسم);

        إذا (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.قدر)) {
            PyUnicodeWriter_Discard(writer);
            أرجع عدم;
        }
    }

    PyObject *final = Py_Unicode_Writer_انت(writer);
    إذا (final == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جوڑو_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(final, قسم, lineno, خانہ_وتھ,
                           سرا_lineno, سرا_خانہ_وتھ, arena);
}

ساكن asdl_اظہاریہ_seq *
_build_concatenated_str(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                               صحيح سرا_خانہ_وتھ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        بدل(elem->قسم) {
            حالة Joined_Str_قسم:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.اقدار);
                اكسر;
            حالة Template_Str_قسم:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.اقدار);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_اظہاریہ_seq* flattened = Py_asdl_اظہاریہ_seq_new(n_flattened_elements, p->arena);
    إذا (flattened == عدم) {
        أرجع عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        بدل(elem->قسم) {
            حالة Joined_Str_قسم:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.اقدار); j++) {
                    اظہاریہ_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.اقدار, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_سیٹ(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة Template_Str_قسم:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.اقدار); j++) {
                    اظہاریہ_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.اقدار, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_سیٹ(flattened, current_pos++, subvalue);
                }
                اكسر;
            افتراضي:
                asdl_seq_سیٹ(flattened, current_pos++, elem);
                اكسر;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحيح prev_is_constant = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        إذا (elem->قسم == Constant_قسم &&
            PyUnicode_CheckExact(elem->v.Constant.قدر) &&
            Py_Unicode_GET_لمائی(elem->v.Constant.قدر) == 0)
            استمر;

        إذا (!prev_is_constant || elem->قسم != Constant_قسم) {
            n_elements++;
        }
        prev_is_constant = elem->قسم == Constant_قسم;
    }

    asdl_اظہاریہ_seq* اقدار = Py_asdl_اظہاریہ_seq_new(n_elements, p->arena);
    إذا (اقدار == عدم) {
        أرجع عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        إذا (elem->قسم == Constant_قسم) {
            إذا (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->قسم == Constant_قسم) {
                اظہاریہ_ty پہلا_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *قسم = elem->v.Constant.قسم;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                إذا (writer == عدم) {
                    أرجع عدم;
                }
                اظہاریہ_ty آخری_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    اظہاریہ_ty current_elem = asdl_seq_GET(flattened, j);
                    إذا (current_elem->قسم == Constant_قسم) {
                        إذا (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.قدر)) {
                            PyUnicodeWriter_Discard(writer);
                            أرجع عدم;
                        }
                        آخری_elem = current_elem;
                    } وإلا {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_انت(writer);
                إذا (concat_str == عدم) {
                    أرجع عدم;
                }
                إذا (Py_Arena_جوڑو_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    أرجع عدم;
                }
                elem = _PyAST_Constant(concat_str, قسم, پہلا_elem->lineno,
                                       پہلا_elem->خانہ_وتھ,
                                       آخری_elem->سرا_lineno,
                                       آخری_elem->سرا_خانہ_وتھ, p->arena);
                إذا (elem == عدم) {
                    أرجع عدم;
                }
            }

            /* Drop all empty contanst strings */
            إذا (PyUnicode_CheckExact(elem->v.Constant.قدر) &&
                Py_Unicode_GET_لمائی(elem->v.Constant.قدر) == 0) {
                استمر;
            }
        }

        asdl_seq_سیٹ(اقدار, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    أرجع اقدار;
}

ساكن اظہاریہ_ty
_build_concatenated_joined_str(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                               صحيح سرا_خانہ_وتھ, PyArena *arena)
{
    asdl_اظہاریہ_seq *اقدار = _build_concatenated_str(p, لڑیاں, lineno,
        خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
    إذا (!اقدار) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(اقدار, lineno, خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, p->arena);
}

اظہاریہ_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                               صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                               صحيح سرا_خانہ_وتھ, PyArena *arena)
{
    asdl_اظہاریہ_seq *اقدار = _build_concatenated_str(p, لڑیاں, lineno,
        خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
    إذا (!اقدار) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(اقدار, lineno, خانہ_وتھ, سرا_lineno,
        سرا_خانہ_وتھ, arena);
}

اظہاریہ_ty
Py_Pegen_concatenate_لڑیاں(Parser *p, asdl_اظہاریہ_seq *لڑیاں,
                             صحيح lineno, صحيح خانہ_وتھ, صحيح سرا_lineno,
                             صحيح سرا_خانہ_وتھ, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(لڑیاں);
    أكد(len > 0);

    صحيح f_string_لبھیا = 0;
    صحيح unicode_string_لبھیا = 0;
    صحيح بائٹاں_لبھیا = 0;

    Py_ssize_t i = 0;
    لكل (i = 0; i < len; i++) {
        اظہاریہ_ty elem = asdl_seq_GET(لڑیاں, i);
        بدل(elem->قسم) {
            حالة Constant_قسم:
                إذا (Py_بائٹاں_Check_Exact(elem->v.Constant.قدر)) {
                    بائٹاں_لبھیا = 1;
                } وإلا {
                    unicode_string_لبھیا = 1;
                }
                اكسر;
            حالة Joined_Str_قسم:
                f_string_لبھیا = 1;
                اكسر;
            حالة Template_Str_قسم:
                // python.gram handles this; we should never get here
                أكد(0);
                اكسر;
            افتراضي:
                f_string_لبھیا = 1;
                اكسر;
        }
    }

    // Cannot mix unicode and bytes
    إذا ((unicode_string_لبھیا || f_string_لبھیا) && بائٹاں_لبھیا) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        أرجع عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    إذا (!f_string_لبھیا) {
        إذا (len == 1) {
            أرجع asdl_seq_GET(لڑیاں, 0);
        }
        وإلا إذا (بائٹاں_لبھیا) {
            أرجع build_concatenated_بائٹاں(p, لڑیاں, lineno, خانہ_وتھ,
                سرا_lineno, سرا_خانہ_وتھ, arena);
        }
        وإلا {
            أرجع _build_concatenated_unicode(p, لڑیاں, lineno, خانہ_وتھ,
                سرا_lineno, سرا_خانہ_وتھ, arena);
        }
    }

    أرجع _build_concatenated_joined_str(p, لڑیاں, lineno,
        خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
}

ساكن صحيح
warn_relative_import_دا_lazy(Parser *p, asdl_seq *dots, اظہاریہ_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    إذا (p->call_invalid_قاعدے) {
        أرجع 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *آخری_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    إذا (
        آخری_dot->سرا_lineno == module->lineno
        && آخری_dot->سرا_خانہ_وتھ == module->خانہ_وتھ
    ) {
        أرجع 0;
    }

    صحيح گنتی = Py_Pegen_seq_گنتی_dots(dots);
    حرف *buf = PyMem_RawMalloc(گنتی + 1);
    إذا (buf == عدم) {
        Py_Err_No_حافظہ();
        أرجع -1;
    }
    memset(buf, '.', گنتی);
    buf[گنتی] = '\0';

    PyObject *پیغام = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    إذا (پیغام == عدم) {
        أرجع -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحيح res = _PyErr_EmitSyntaxWarning(پیغام,
                                       info.filename,
                                       module->lineno,
                                       module->خانہ_وتھ + 1,
                                       module->سرا_lineno,
                                       module->سرا_خانہ_وتھ + 1,
                                       info.module);
    Py_DECREF(پیغام);
    أرجع res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, اظہاریہ_ty module_name,
                             asdl_alias_seq *ناں, اظہاریہ_ty lazy_token, صحيح lineno,
                             صحيح خانہ_وتھ, صحيح سرا_lineno, صحيح سرا_خانہ_وتھ,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = Py_Pegen_seq_گنتی_dots(dots);
    إذا (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        إذا (lazy_token) {
            RAISE_SYNTAX_ERROR_معلوم_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < asdl_seq_LEN(ناں); i++) {
            alias_ty alias = asdl_seq_GET(ناں, i);
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
        إذا (warn_relative_import_دا_lazy(p, dots, module_name) < 0) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_ImportFrom(module, ناں, level, lazy_token ? 1 : 0, lineno,
                             خانہ_وتھ, سرا_lineno, سرا_خانہ_وتھ, arena);
}

asdl_stmt_seq*
Py_Pegen_رجسٹر_stmts(Parser *p, asdl_stmt_seq* stmts) {
    إذا (!p->call_invalid_قاعدے) {
        أرجع stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    إذا (len == 0) {
        أرجع stmts;
    }
    stmt_ty آخری_stmt = asdl_seq_GET(stmts, len - 1);
    إذا (p->آخری_stmt_location.lineno > آخری_stmt->lineno) {
        أرجع stmts;
    }
    p->آخری_stmt_location.lineno = آخری_stmt->lineno;
    p->آخری_stmt_location.خانہ_وتھ = آخری_stmt->خانہ_وتھ;
    p->آخری_stmt_location.سرا_lineno = آخری_stmt->سرا_lineno;
    p->آخری_stmt_location.سرا_خانہ_وتھ = آخری_stmt->سرا_خانہ_وتھ;
    أرجع stmts;
}
