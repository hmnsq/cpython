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
    asdl_seq_سېټ_UNTYPED(seq, 0, a);
    أرجع seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_ورننباسه_دننه_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    إذا (!seq) {
        أرجع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    asdl_seq_سېټ_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_سېټ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    أرجع new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_منزل_پای_ټکی(Parser *p, asdl_seq *seq, فراغ *a)
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
        asdl_seq_سېټ_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_سېټ_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    أرجع new_seq;
}

ساكن Py_ssize_t
get_flattened_seq_اندازه(asdl_seq *seqs)
{
    Py_ssize_t اندازه = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        اندازه += asdl_seq_LEN(inner_seq);
    }
    أرجع اندازه;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_اندازه = get_flattened_seq_اندازه(seqs);
    أكد(flattened_seq_اندازه > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_اندازه, p->arena);
    إذا (!flattened_seq) {
        أرجع عدم;
    }

    صحيح flattened_seq_idx = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        لكل (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_سېټ_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_اندازه);

    أرجع flattened_seq;
}

فراغ *
Py_Pegen_seq_وروستی_توکی(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أرجع asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
Py_Pegen_seq_لومړی_توکی(asdl_seq *seq)
{
    أرجع asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
بیان_ty
Py_Pegen_join_نومونه_with_dot(Parser *p, بیان_ty لومړی_name, بیان_ty دوهم_name)
{
    أكد(لومړی_name != عدم && دوهم_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            لومړی_name->v.Name.id, دوهم_name->v.Name.id);
    إذا (!uni) {
        أرجع عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        أرجع عدم;
    }

    أرجع _PyAST_Name(uni, بارول, EXTRA_بیان(لومړی_name, دوهم_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
Py_Pegen_seq_شمېر_dots(asdl_seq *seq)
{
    صحيح شمېره_د_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_بیان = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_بیان->type) {
            حالة ELLIPSIS:
                شمېره_د_dots += 3;
                اكسر;
            حالة DOT:
                شمېره_د_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    أرجع شمېره_د_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_ستوری(Parser *p, صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                        صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    إذا (!str) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_alias(str, عدم, lineno, ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_نومونه_منزل_ids(Parser *p, asdl_بیان_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty e = asdl_seq_GET(seq, i);
        asdl_seq_سېټ(new_seq, i, e->v.Name.id);
    }
    أرجع new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_بیان_Pair *
Py_Pegen_cmpop_بیان_pair(Parser *p, cmpop_ty cmpop, بیان_ty بیان)
{
    أكد(بیان != عدم);
    Cmpop_بیان_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_بیان_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->cmpop = cmpop;
    a->بیان = بیان;
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
        Cmpop_بیان_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->cmpop);
    }
    أرجع new_seq;
}

asdl_بیان_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_بیان_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->بیان);
    }
    أرجع new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_بیان_seq *
سېټ_seq_context(Parser *p, asdl_بیان_seq *seq, بیان_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إذا (len == 0) {
        أرجع عدم;
    }

    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty e = asdl_seq_GET(seq, i);
        بیان_ty new_e = Py_Pegen_سېټ_بیان_context(p, e, ctx);
        إذا (!new_e) {
            أرجع عدم;
        }
        asdl_seq_سېټ(new_seq, i, new_e);
    }
    أرجع new_seq;
}

ساكن بیان_ty
سېټ_name_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    أرجع _PyAST_Name(e->v.Name.id, ctx, EXTRA_بیان(e, e));
}

ساكن بیان_ty
سېټ_tuple_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    asdl_بیان_seq *seq = سېټ_seq_context(p, e->v.Tuple.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_Tuple(seq, ctx, EXTRA_بیان(e, e));
}

ساكن بیان_ty
سېټ_list_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    asdl_بیان_seq *seq = سېټ_seq_context(p, e->v.List.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_List(seq, ctx, EXTRA_بیان(e, e));
}

ساكن بیان_ty
سېټ_subscript_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    أرجع _PyAST_Subscript(e->v.Subscript.ارزښت, e->v.Subscript.ټوټه,
                            ctx, EXTRA_بیان(e, e));
}

ساكن بیان_ty
سېټ_attribute_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    أرجع _PyAST_Attribute(e->v.Attribute.ارزښت, e->v.Attribute.attr,
                            ctx, EXTRA_بیان(e, e));
}

ساكن بیان_ty
سېټ_starred_context(Parser *p, بیان_ty e, بیان_context_ty ctx)
{
    بیان_ty inner = Py_Pegen_سېټ_بیان_context(p, e->v.Starred.ارزښت, ctx);
    إذا (!inner) {
        أرجع عدم;
    }
    أرجع _PyAST_Starred(inner, ctx, EXTRA_بیان(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
بیان_ty
Py_Pegen_سېټ_بیان_context(Parser *p, بیان_ty بیان, بیان_context_ty ctx)
{
    أكد(بیان != عدم);

    بیان_ty new = عدم;
    بدل (بیان->ډول) {
        حالة Name_ډول:
            new = سېټ_name_context(p, بیان, ctx);
            اكسر;
        حالة Tuple_ډول:
            new = سېټ_tuple_context(p, بیان, ctx);
            اكسر;
        حالة List_ډول:
            new = سېټ_list_context(p, بیان, ctx);
            اكسر;
        حالة Subscript_ډول:
            new = سېټ_subscript_context(p, بیان, ctx);
            اكسر;
        حالة Attribute_ډول:
            new = سېټ_attribute_context(p, بیان, ctx);
            اكسر;
        حالة Starred_ډول:
            new = سېټ_starred_context(p, بیان, ctx);
            اكسر;
        افتراضي:
            new = بیان;
    }
    أرجع new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
بیان_ty
Py_Pegen_make_موخه(Parser *p, بیان_ty بیان, TARGETS_TYPE targets_type)
{
    أكد(بیان != عدم);
    أكد(targets_type != FOR_TARGETS);
    إذا (Py_Pegen_get_invalid_موخه(بیان, targets_type) != عدم) {
        أرجع عدم;
    }
    أرجع Py_Pegen_سېټ_بیان_context(
        p, بیان, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
کلی_ارزښت_Pair *
Py_Pegen_کلی_ارزښت_pair(Parser *p, بیان_ty کلی, بیان_ty ارزښت)
{
    کلی_ارزښت_Pair *a = _PyArena_Malloc(p->arena, حجم(کلی_ارزښت_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلی = کلی;
    a->ارزښت = ارزښت;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_بیان_seq *
Py_Pegen_get_کلیګانې(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلی_ارزښت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->کلی);
    }
    أرجع new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_بیان_seq *
Py_Pegen_get_ارزښتونه(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلی_ارزښت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->ارزښت);
    }
    أرجع new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
کلی_نمونه_Pair *
Py_Pegen_کلی_نمونه_pair(Parser *p, بیان_ty کلی, نمونه_ty نمونه)
{
    کلی_نمونه_Pair *a = _PyArena_Malloc(p->arena, حجم(کلی_نمونه_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلی = کلی;
    a->نمونه = نمونه;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_بیان_seq *
Py_Pegen_get_نمونه_کلیګانې(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلی_نمونه_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->کلی);
    }
    أرجع new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_نمونه_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_نمونه_seq *new_seq = Py_asdl_نمونه_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلی_نمونه_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_سېټ(new_seq, i, pair->نمونه);
    }
    أرجع new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, بیان_ty ارزښت, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    إذا (!a) {
        أرجع عدم;
    }
    a->arg = Py_Pegen_جمع_کړه_type_comment_منزل_arg(p, arg, tc);
    إذا (!a->arg) {
        أرجع عدم;
    }
    a->ارزښت = ارزښت;
    أرجع a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_نومونه, asdl_seq *نومونه_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    إذا (!a) {
        أرجع عدم;
    }
    a->plain_نومونه = plain_نومونه;
    a->نومونه_with_defaults = نومونه_with_defaults;
    أرجع a;
}

/* Constructs a StarEtc */
ستوری_Etc *
Py_Pegen_ستوری_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    ستوری_Etc *a = _PyArena_Malloc(p->arena, حجم(ستوری_Etc));
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
    Py_ssize_t لومړی_len = asdl_seq_LEN(a);
    Py_ssize_t دوهم_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(لومړی_len + دوهم_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < لومړی_len; i++) {
        asdl_seq_سېټ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < دوهم_len; i++) {
        asdl_seq_سېټ_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    أرجع new_seq;
}

ساكن asdl_arg_seq*
get_نومونه(Parser *p, asdl_seq *نومونه_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نومونه_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نومونه_with_defaults, i);
        asdl_seq_سېټ(seq, i, pair->arg);
    }
    أرجع seq;
}

ساكن asdl_بیان_seq *
_get_defaults(Parser *p, asdl_seq *نومونه_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نومونه_with_defaults);
    asdl_بیان_seq *seq = Py_asdl_بیان_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نومونه_with_defaults, i);
        asdl_seq_سېټ(seq, i, pair->ارزښت);
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
        asdl_arg_seq *slash_with_default_نومونه =
                get_نومونه(p, slash_with_default->نومونه_with_defaults);
        إذا (!slash_with_default_نومونه) {
            أرجع -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_نومونه,
                (asdl_seq*)slash_with_default_نومونه);
    }
    وإلا {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    أرجع *posonlyargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_نومونه,
              asdl_seq *نومونه_with_default,
              asdl_arg_seq **posargs) {

    إذا (نومونه_with_default != عدم) {
        إذا (plain_نومونه != عدم) {
            asdl_arg_seq *نومونه_with_default_نومونه = get_نومونه(p, نومونه_with_default);
            إذا (!نومونه_with_default_نومونه) {
                أرجع -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_نومونه, (asdl_seq*)نومونه_with_default_نومونه);
        }
        وإلا {
            *posargs = get_نومونه(p, نومونه_with_default);
        }
    }
    وإلا {
        إذا (plain_نومونه != عدم) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_نومونه;
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
                  asdl_seq *نومونه_with_default,
                  asdl_بیان_seq **posdefaults) {
    إذا (slash_with_default != عدم && نومونه_with_default != عدم) {
        asdl_بیان_seq *slash_with_default_ارزښتونه =
                _get_defaults(p, slash_with_default->نومونه_with_defaults);
        إذا (!slash_with_default_ارزښتونه) {
            أرجع -1;
        }
        asdl_بیان_seq *نومونه_with_default_ارزښتونه = _get_defaults(p, نومونه_with_default);
        إذا (!نومونه_with_default_ارزښتونه) {
            أرجع -1;
        }
        *posdefaults = (asdl_بیان_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_ارزښتونه,
                (asdl_seq*)نومونه_with_default_ارزښتونه);
    }
    وإلا إذا (slash_with_default == عدم && نومونه_with_default != عدم) {
        *posdefaults = _get_defaults(p, نومونه_with_default);
    }
    وإلا إذا (slash_with_default != عدم && نومونه_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->نومونه_with_defaults);
    }
    وإلا {
        *posdefaults = Py_asdl_بیان_seq_new(0, p->arena);
    }
    أرجع *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, ستوری_Etc *ستوری_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_بیان_seq **kwdefaults) {
    إذا (ستوری_etc != عدم && ستوری_etc->kwonlyargs != عدم) {
        *kwonlyargs = get_نومونه(p, ستوری_etc->kwonlyargs);
    }
    وإلا {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    إذا (*kwonlyargs == عدم) {
        أرجع -1;
    }

    إذا (ستوری_etc != عدم && ستوری_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, ستوری_etc->kwonlyargs);
    }
    وإلا {
        *kwdefaults = Py_asdl_بیان_seq_new(0, p->arena);
    }

    إذا (*kwdefaults == عدم) {
        أرجع -1;
    }

    أرجع 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_نومونه,
                        asdl_seq *نومونه_with_default, ستوری_Etc *ستوری_etc)
{
    asdl_arg_seq *posonlyargs;
    إذا (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        أرجع عدم;
    }

    asdl_arg_seq *posargs;
    إذا (_make_posargs(p, plain_نومونه, نومونه_with_default, &posargs) == -1) {
        أرجع عدم;
    }

    asdl_بیان_seq *posdefaults;
    إذا (_make_posdefaults(p,slash_with_default, نومونه_with_default, &posdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty vararg = عدم;
    إذا (ستوری_etc != عدم && ستوری_etc->vararg != عدم) {
        vararg = ستوری_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_بیان_seq *kwdefaults;
    إذا (_make_kwargs(p, ستوری_etc, &kwonlyargs, &kwdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty kwarg = عدم;
    إذا (ستوری_etc != عدم && ستوری_etc->kwarg != عدم) {
        kwarg = ستوری_etc->kwarg;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_تشه_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posonlyargs) {
        أرجع عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posargs) {
        أرجع عدم;
    }
    asdl_بیان_seq *posdefaults = Py_asdl_بیان_seq_new(0, p->arena);
    إذا (!posdefaults) {
        أرجع عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!kwonlyargs) {
        أرجع عدم;
    }
    asdl_بیان_seq *kwdefaults = Py_asdl_بیان_seq_new(0, p->arena);
    إذا (!kwdefaults) {
        أرجع عدم;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty ډول)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    إذا (!a) {
        أرجع عدم;
    }
    a->ډول = ډول;
    أرجع a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_بیان_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    إذا (function_def->ډول == Async_Function_Def_ډول) {
        أرجع _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.دلیلونه,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->ستون_دننه_واټن,
            function_def->پای_ټکی_lineno, function_def->پای_ټکی_ستون_دننه_واټن, p->arena);
    }

    أرجع _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.دلیلونه,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->ستون_دننه_واټن,
        function_def->پای_ټکی_lineno, function_def->پای_ټکی_ستون_دننه_واټن, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_بیان_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != عدم);
    أرجع _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->ستون_دننه_واټن, class_def->پای_ټکی_lineno,
        class_def->پای_ټکی_ستون_دننه_واټن, p->arena);
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
seq_شمېره_د_starred_exprs(asdl_seq *seq)
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
asdl_بیان_seq *
Py_Pegen_seq_ایستل_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = seq_شمېره_د_starred_exprs(kwargs);
    إذا (new_len == 0) {
        أرجع عدم;
    }
    asdl_بیان_seq *new_seq = Py_asdl_بیان_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (!k->is_keyword) {
            asdl_seq_سېټ(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_شمېره_د_starred_exprs(kwargs);
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
            asdl_seq_سېټ(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

بیان_ty
_PyPegen_ensure_imaginary(Parser *p, بیان_ty توانیز_تابع)
{
    إذا (توانیز_تابع->ډول != Constant_ډول || !PyComplex_CheckExact(توانیز_تابع->v.Constant.ارزښت)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(توانیز_تابع, "imaginary number required in complex literal");
        أرجع عدم;
    }
    أرجع توانیز_تابع;
}

بیان_ty
_PyPegen_ensure_real(Parser *p, بیان_ty توانیز_تابع)
{
    إذا (توانیز_تابع->ډول != Constant_ډول || PyComplex_CheckExact(توانیز_تابع->v.Constant.ارزښت)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(توانیز_تابع, "real number required in complex literal");
        أرجع عدم;
    }
    أرجع توانیز_تابع;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t عدد = p->type_ignore_comments.عدد_توکي;
    إذا (عدد > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(عدد, p->arena);
        إذا (type_ignores == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < عدد; i++) {
            PyObject *نښان = _PyPegen_new_type_comment(p, p->type_ignore_comments.توکي[i].comment);
            إذا (نښان == عدم) {
                أرجع عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.توکي[i].lineno,
                                                  نښان, p->arena);
            إذا (ti == عدم) {
                أرجع عدم;
            }
            asdl_seq_سېټ(type_ignores, i, ti);
        }
    }
    أرجع _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, مزي_اوږدوالی(s), عدم);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع res;
}

arg_ty
Py_Pegen_جمع_کړه_type_comment_منزل_arg(Parser *p, arg_ty a, Token *tc)
{
    إذا (tc == عدم) {
        أرجع a;
    }
    ثابت حرف *بایټونه = Py_بایټونه_As_String(tc->بایټونه);
    إذا (بایټونه == عدم) {
        أرجع عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بایټونه);
    إذا (tco == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->ستون_دننه_واټن, a->پای_ټکی_lineno, a->پای_ټکی_ستون_دننه_واټن,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحيح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->بایټونه != عدم);
    أكد(t->type == NOTEQUAL);

    ثابت حرف* tok_str = Py_بایټونه_AS_STRING(t->بایټونه);
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
_PyPegen_check_legacy_stmt(Parser *p, بیان_ty name) {
    إذا (name->ډول != Name_ډول) {
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
_PyPegen_raise_error_for_missing_comma(Parser *p, بیان_ty a, بیان_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    إذا (_PyPegen_check_legacy_stmt(p, a)) {
        أرجع عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    إذا (p->نښې[p->mark - 1]->level == 0) {
        أرجع عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    إذا (a->پای_ټکی_lineno > a->lineno) {
        أرجع RAISE_ERROR_معلوم_LOCATION(
            p, PyExc_SyntaxError, a->پای_ټکی_lineno, a->ستون_دننه_واټن,
            a->پای_ټکی_lineno, a->پای_ټکی_ستون_دننه_واټن,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    أرجع RAISE_ERROR_معلوم_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->ستون_دننه_واټن,
        b->پای_ټکی_lineno, b->پای_ټکی_ستون_دننه_واټن,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساكن پایله_Token_With_Metadata *
پایله_token_with_metadata(Parser *p, فراغ *پایله, PyObject *metadata)
{
    پایله_Token_With_Metadata *res = _PyArena_Malloc(p->arena, حجم(پایله_Token_With_Metadata));
    إذا (res == عدم) {
        أرجع عدم;
    }
    res->metadata = metadata;
    res->پایله = پایله;
    أرجع res;
}

ساكن حرف
formatted_string_مختاړی(ثابت Parser *p)
{
    صحيح nested = 0;
    لكل (صحيح i = p->mark - 1; i >= 0; i--) {
        صحيح type = p->نښې[i]->type;
        إذا (type == FSTRING_پای_ټکی || type == TSTRING_پای_ټکی) {
            nested++;
        }
        وإلا إذا (type == FSTRING_پیل || type == TSTRING_پیل) {
            إذا (nested == 0) {
                أرجع type == TSTRING_پیل ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

پایله_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, بیان_ty conv)
{
    إذا (conv_token->lineno != conv->lineno || conv_token->پای_ټکی_ستون_دننه_واټن != conv->ستون_دننه_واټن) {
        أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_مختاړی(p)
        );
    }

    Py_UCS4 لومړی = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    إذا (Py_Unicode_GET_اوږدتیا(conv->v.Name.id) > 1 ||
            !(لومړی == 's' || لومړی == 'r' || لومړی == 'a')) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_مختاړی(p),
                                            conv->v.Name.id);
        أرجع عدم;
    }

    أرجع پایله_token_with_metadata(p, conv, conv_token->metadata);
}

پایله_Token_With_Metadata *
Py_Pegen_تیاری_ډک_format_spec(Parser *p, Token *colon, asdl_بیان_seq *spec, صحيح lineno, صحيح ستون_دننه_واټن,
                                صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena)
{
    إذا (!spec) {
        أرجع عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_توکي = asdl_seq_LEN(spec);
    Py_ssize_t non_تشه_شمېر = 0;
    لكل (Py_ssize_t i = 0; i < n_توکي; i++) {
        بیان_ty توکی = asdl_seq_GET(spec, i);
        non_تشه_شمېر += !(توکی->ډول == Constant_ډول &&
                             PyUnicode_CheckExact(توکی->v.Constant.ارزښت) &&
                             Py_Unicode_GET_اوږدتیا(توکی->v.Constant.ارزښت) == 0);
    }
    إذا (non_تشه_شمېر != n_توکي) {
        asdl_بیان_seq *resized_spec =
            Py_asdl_بیان_seq_new(non_تشه_شمېر, p->arena);
        إذا (resized_spec == عدم) {
            أرجع عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_توکي; i++) {
            بیان_ty توکی = asdl_seq_GET(spec, i);
            إذا (توکی->ډول == Constant_ډول &&
                PyUnicode_CheckExact(توکی->v.Constant.ارزښت) &&
                Py_Unicode_GET_اوږدتیا(توکی->v.Constant.ارزښت) == 0) {
                استمر;
            }
            asdl_seq_سېټ(resized_spec, j++, توکی);
        }
        أكد(j == non_تشه_شمېر);
        spec = resized_spec;
    }
    بیان_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    إذا (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->ډول == Constant_ډول)) {
        res = _PyAST_JoinedStr(spec, lineno, ستون_دننه_واټن, پای_ټکی_lineno,
                                    پای_ټکی_ستون_دننه_واټن, p->arena);
    } وإلا {
        res = Py_Pegen_concatenate_مزي_ګانې(p, spec,
                             lineno, ستون_دننه_واټن, پای_ټکی_lineno,
                             پای_ټکی_ستون_دننه_واټن, arena);
    }
    إذا (!res) {
        أرجع عدم;
    }
    أرجع پایله_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_بیان_name(بیان_ty e)
{
    أكد(e != عدم);
    بدل (e->ډول) {
        حالة Attribute_ډول:
            أرجع "attribute";
        حالة Subscript_ډول:
            أرجع "subscript";
        حالة Starred_ډول:
            أرجع "starred";
        حالة Name_ډول:
            أرجع "name";
        حالة List_ډول:
            أرجع "list";
        حالة Tuple_ډول:
            أرجع "tuple";
        حالة Lambda_ډول:
            أرجع "lambda";
        حالة Call_ډول:
            أرجع "function call";
        حالة Bool_Op_ډول:
        حالة Bin_Op_ډول:
        حالة Unary_Op_ډول:
            أرجع "expression";
        حالة Generator_Exp_ډول:
            أرجع "generator expression";
        حالة Yield_ډول:
        حالة Yield_From_ډول:
            أرجع "yield expression";
        حالة Await_ډول:
            أرجع "await expression";
        حالة List_Comp_ډول:
            أرجع "list comprehension";
        حالة سېټ_Comp_ډول:
            أرجع "set comprehension";
        حالة Dict_Comp_ډول:
            أرجع "dict comprehension";
        حالة Dict_ډول:
            أرجع "dict literal";
        حالة سېټ_ډول:
            أرجع "set display";
        حالة Joined_Str_ډول:
        حالة Formatted_ارزښت_ډول:
            أرجع "f-string expression";
        حالة Template_Str_ډول:
        حالة Interpolation_ډول:
            أرجع "t-string expression";
        حالة Constant_ډول: {
            PyObject *ارزښت = e->v.Constant.ارزښت;
            إذا (ارزښت == Py_None) {
                أرجع "None";
            }
            إذا (ارزښت == Py_False) {
                أرجع "False";
            }
            إذا (ارزښت == Py_True) {
                أرجع "True";
            }
            إذا (ارزښت == Py_Ellipsis) {
                أرجع "ellipsis";
            }
            أرجع "literal";
        }
        حالة Compare_ډول:
            أرجع "comparison";
        حالة If_Exp_ډول:
            أرجع "conditional expression";
        حالة Named_بیان_ډول:
            أرجع "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->ډول, e->lineno);
            أرجع عدم;
    }
}

بیان_ty
Py_Pegen_get_وروستی_comprehension_توکی(comprehension_ty comprehension) {
    إذا (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        أرجع comprehension->iter;
    }
    أرجع Py_Pegen_وروستی_توکی(comprehension->ifs, بیان_ty);
}

بیان_ty _PyPegen_collect_call_seqs(Parser *p, asdl_بیان_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                     صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena) {
    Py_ssize_t دلیلونه_len = asdl_seq_LEN(a);
    Py_ssize_t ټول_len = دلیلونه_len;

    إذا (b == عدم) {
        أرجع _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, ستون_دننه_واټن,
                        پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);

    }

    asdl_بیان_seq *starreds = Py_Pegen_seq_ایستل_starred_exprs(p, b);
    إذا (!starreds && PyErr_Occurred()) {
        أرجع عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    إذا (!keywords && PyErr_Occurred()) {
        أرجع عدم;
    }

    إذا (starreds) {
        ټول_len += asdl_seq_LEN(starreds);
    }

    asdl_بیان_seq *دلیلونه = Py_asdl_بیان_seq_new(ټول_len, arena);
    إذا (دلیلونه == عدم) {
        أرجع عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < دلیلونه_len; i++) {
        asdl_seq_سېټ(دلیلونه, i, asdl_seq_GET(a, i));
    }
    لكل (; i < ټول_len; i++) {
        asdl_seq_سېټ(دلیلونه, i, asdl_seq_GET(starreds, i - دلیلونه_len));
    }

    أرجع _PyAST_Call(_PyPegen_dummy_name(p), دلیلونه, keywords, lineno,
                       ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
}

// AST Error reporting helpers

بیان_ty
Py_Pegen_get_invalid_موخه(بیان_ty e, TARGETS_TYPE targets_type)
{
    إذا (e == عدم) {
        أرجع عدم;
    }

    إذا (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        إذا (targets_type == SINGLE_TARGETS && e->ډول == Name_ډول) {
            أرجع عدم;
        }
        أرجع e->ډول == Attribute_ډول || e->ډول == Subscript_ډول ? عدم : e;
    }

#تعریف_کړه لیدنه_CONTAINER(CONTAINER, TYPE) افعل { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            بیان_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            بیان_ty ماشوم = Py_Pegen_get_invalid_موخه(other, targets_type);\
            إذا (ماشوم != عدم) {\
                أرجع ماشوم;\
            }\
        }\
    } طالما (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->ډول) {
        حالة List_ډول:
            لیدنه_CONTAINER(e, List);
            أرجع عدم;
        حالة Tuple_ډول:
            لیدنه_CONTAINER(e, Tuple);
            أرجع عدم;
        حالة Starred_ډول:
            إذا (targets_type == DEL_TARGETS) {
                أرجع e;
            }
            أرجع Py_Pegen_get_invalid_موخه(e->v.Starred.ارزښت, targets_type);
        حالة Compare_ډول:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            إذا (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                إذا (cmpop == دننه) {
                    أرجع Py_Pegen_get_invalid_موخه(e->v.Compare.کیڼ, targets_type);
                }
                أرجع عدم;
            }
            أرجع e;
        حالة Name_ډول:
        حالة Subscript_ډول:
        حالة Attribute_ډول:
            أرجع عدم;
        افتراضي:
            أرجع e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, بیان_ty e) {
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
Py_Pegen_nonparen_genexp_دننه_call(Parser *p, بیان_ty دلیلونه, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(دلیلونه->v.Call.دلیلونه);
    إذا (len <= 1) {
        أرجع عدم;
    }

    comprehension_ty وروستی_comprehension = Py_Pegen_وروستی_توکی(comprehensions, comprehension_ty);

    أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
        (بیان_ty) asdl_seq_GET(دلیلونه->v.Call.دلیلونه, len - 1),
        Py_Pegen_get_وروستی_comprehension_توکی(وروستی_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن بیان_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, بیان_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.ارزښت));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.ارزښت);
    إذا (bstr == عدم) {
        أرجع عدم;
    }

    اندازه_t len;
    إذا (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } وإلا {
        len = مزي_اوږدوالی(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == عدم;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    إذا (str == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, constant->lineno, constant->ستون_دننه_واټن,
                           constant->پای_ټکی_lineno, constant->پای_ټکی_ستون_دننه_واټن,
                           p->arena);
}

ساكن asdl_بیان_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_بیان_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_توکي = asdl_seq_LEN(raw_expressions);
    Py_ssize_t ټول_توکي = n_توکي;
    لكل (Py_ssize_t i = 0; i < n_توکي; i++) {
        بیان_ty توکی = asdl_seq_GET(raw_expressions, i);
        إذا (توکی->ډول == Joined_Str_ډول) {
            ټول_توکي += asdl_seq_LEN(توکی->v.JoinedStr.ارزښتونه) - 1;
        }
    }

    ثابت حرف* quote_str = Py_بایټونه_As_String(a->بایټونه);
    إذا (quote_str == عدم) {
        أرجع عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_بیان_seq *seq = Py_asdl_بیان_seq_new(ټول_توکي, p->arena);
    إذا (seq == عدم) {
        أرجع عدم;
    }

    Py_ssize_t شاخص = 0;
    لكل (Py_ssize_t i = 0; i < n_توکي; i++) {
        بیان_ty توکی = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        إذا (توکی->ډول == Joined_Str_ډول) {
            asdl_بیان_seq *ارزښتونه = توکی->v.JoinedStr.ارزښتونه;
            إذا (asdl_seq_LEN(ارزښتونه) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             توکی->lineno);
                أرجع عدم;
            }

            بیان_ty لومړی = asdl_seq_GET(ارزښتونه, 0);
            أكد(لومړی->ډول == Constant_ډول);
            asdl_seq_سېټ(seq, شاخص++, لومړی);

            بیان_ty دوهم = asdl_seq_GET(ارزښتونه, 1);
            أكد((is_tstring &&
                    دوهم->ډول == Interpolation_ډول) ||
                   دوهم->ډول == Formatted_ارزښت_ډول);
            asdl_seq_سېټ(seq, شاخص++, دوهم);

            استمر;
        }

        إذا (توکی->ډول == Constant_ډول) {
            توکی = _PyPegen_decode_fstring_part(p, is_raw, توکی, b);
            إذا (توکی == عدم) {
                أرجع عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            إذا (PyUnicode_CheckExact(توکی->v.Constant.ارزښت)
                && Py_Unicode_GET_اوږدتیا(توکی->v.Constant.ارزښت) == 0) {
                استمر;
            }
        }
        asdl_seq_سېټ(seq, شاخص++, توکی);
    }

    asdl_بیان_seq *resized_exprs;
    إذا (شاخص != ټول_توکي) {
        resized_exprs = Py_asdl_بیان_seq_new(شاخص, p->arena);
        إذا (resized_exprs == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < شاخص; i++) {
            asdl_seq_سېټ(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وإلا {
        resized_exprs = seq;
    }
    أرجع resized_exprs;
}

بیان_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_بیان_seq *raw_expressions, Token *b) {

    asdl_بیان_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(resized_exprs, a->lineno, a->ستون_دننه_واټن,
                              b->پای_ټکی_lineno, b->پای_ټکی_ستون_دننه_واټن,
                              p->arena);
}

بیان_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_بیان_seq* raw_expressions, Token*b) {

    asdl_بیان_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(resized_exprs, a->lineno, a->ستون_دننه_واټن,
                            b->پای_ټکی_lineno, b->پای_ټکی_ستون_دننه_واټن,
                            p->arena);
}

بیان_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بایټونه == عدم) {
        أرجع Py_Pegen_raise_incomplete_ننوت(p);
    }
    Py_ssize_t bsize = Py_بایټونه_GET_اندازه(tok->بایټونه);
    ثابت حرف *bstr = Py_بایټونه_AS_STRING(tok->بایټونه);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->ستون_دننه_واټن,
                           tok->پای_ټکی_lineno, tok->پای_ټکی_ستون_دننه_واټن,
                           p->arena);
}

بیان_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بایټونه == عدم) {
        أرجع Py_Pegen_raise_incomplete_ننوت(p);
    }
    ثابت حرف *bstr = Py_بایټونه_AS_STRING(tok->بایټونه);
    PyObject* str = PyUnicode_FromString(bstr);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->ستون_دننه_واټن,
                           tok->پای_ټکی_lineno, tok->پای_ټکی_ستون_دننه_واټن,
                           p->arena);
}

بیان_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    إذا (tok->بایټونه == عدم) {
        أرجع Py_Pegen_raise_incomplete_ننوت(p);
    }
    ثابت حرف *the_str = Py_بایټونه_AS_STRING(tok->بایټونه);
    PyObject *s = _PyPegen_parse_string(p, tok);
    إذا (s == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        أرجع عدم;
    }
    PyObject *ډول = عدم;
    إذا (the_str && the_str[0] == 'u') {
        ډول = _PyPegen_new_identifier(p, "u");
        إذا (ډول == عدم) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_Constant(s, ډول, tok->lineno, tok->ستون_دننه_واټن, tok->پای_ټکی_lineno, tok->پای_ټکی_ستون_دننه_واټن, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, پایله_Token_With_Metadata *conversion,
                              پایله_Token_With_Metadata *format)
{
    إذا (conversion != عدم) {
        بیان_ty conversion_بیان = (بیان_ty) conversion->پایله;
        أكد(conversion_بیان->ډول == Name_ډول);
        Py_UCS4 لومړی = PyUnicode_READ_CHAR(conversion_بیان->v.Name.id, 0);
        أرجع Py_خوندي_DOWNCAST(لومړی, Py_UCS4, صحيح);
    }
    وإلا إذا (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        أرجع (صحيح)'r';
    }
    أرجع -1;
}

ساكن PyObject *
strip_interpolation_debug_بیان(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_اوږدتیا(exprstr);

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

بیان_ty _PyPegen_interpolation(Parser *p, بیان_ty افاده, Token *debug, پایله_Token_With_Metadata *conversion,
                                 پایله_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح ستون_دننه_واټن,
                                 صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_پای_ټکی_کرښه, debug_پای_ټکی_دننه_واټن;
    PyObject *debug_metadata;
    constant exprstr;

    إذا (conversion) {
        debug_پای_ټکی_کرښه = ((بیان_ty) conversion->پایله)->lineno;
        debug_پای_ټکی_دننه_واټن = ((بیان_ty) conversion->پایله)->ستون_دننه_واټن;
        debug_metadata = exprstr = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_پای_ټکی_کرښه = ((بیان_ty) format->پایله)->lineno;
        debug_پای_ټکی_دننه_واټن = ((بیان_ty) format->پایله)->ستون_دننه_واټن + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وإلا {
        debug_پای_ټکی_کرښه = پای_ټکی_lineno;
        debug_پای_ټکی_دننه_واټن = پای_ټکی_ستون_دننه_واټن;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_بیان(exprstr)
        : Py_NewRef(exprstr);
    إذا (!final_exprstr || Py_Arena_جمع_کړه_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        أرجع عدم;
    }

    بیان_ty interpolation = _PyAST_Interpolation(
        افاده, final_exprstr, conversion_val, format ? (بیان_ty) format->پایله : عدم,
        lineno, ستون_دننه_واټن, پای_ټکی_lineno,
        پای_ټکی_ستون_دننه_واټن, arena
    );

    إذا (!interpolation || !debug) {
        أرجع interpolation;
    }

    بیان_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, ستون_دننه_واټن + 1, debug_پای_ټکی_کرښه,
                                            debug_پای_ټکی_دننه_واټن - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_بیان_seq *ارزښتونه = Py_asdl_بیان_seq_new(2, arena);
    إذا (!ارزښتونه) {
        أرجع عدم;
    }
    asdl_seq_سېټ(ارزښتونه, 0, debug_متن);
    asdl_seq_سېټ(ارزښتونه, 1, interpolation);
    أرجع _PyAST_JoinedStr(ارزښتونه, lineno, ستون_دننه_واټن, debug_پای_ټکی_کرښه, debug_پای_ټکی_دننه_واټن, p->arena);
}

بیان_ty Py_Pegen_formatted_ارزښت(Parser *p, بیان_ty افاده, Token *debug, پایله_Token_With_Metadata *conversion,
                                 پایله_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح ستون_دننه_واټن,
                                 صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    بیان_ty formatted_ارزښت = Py_AST_Formatted_ارزښت(
        افاده, conversion_val, format ? (بیان_ty) format->پایله : عدم,
        lineno, ستون_دننه_واټن, پای_ټکی_lineno,
        پای_ټکی_ستون_دننه_واټن, arena
    );

    إذا (!formatted_ارزښت || !debug) {
        أرجع formatted_ارزښت;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_پای_ټکی_کرښه, debug_پای_ټکی_دننه_واټن;
    PyObject *debug_metadata;

    إذا (conversion) {
        debug_پای_ټکی_کرښه = ((بیان_ty) conversion->پایله)->lineno;
        debug_پای_ټکی_دننه_واټن = ((بیان_ty) conversion->پایله)->ستون_دننه_واټن;
        debug_metadata = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_پای_ټکی_کرښه = ((بیان_ty) format->پایله)->lineno;
        debug_پای_ټکی_دننه_واټن = ((بیان_ty) format->پایله)->ستون_دننه_واټن + 1;
        debug_metadata = format->metadata;
    }
    وإلا {
        debug_پای_ټکی_کرښه = پای_ټکی_lineno;
        debug_پای_ټکی_دننه_واټن = پای_ټکی_ستون_دننه_واټن;
        debug_metadata = closing_brace->metadata;
    }
    بیان_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, ستون_دننه_واټن + 1, debug_پای_ټکی_کرښه,
                                            debug_پای_ټکی_دننه_واټن - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_بیان_seq *ارزښتونه = Py_asdl_بیان_seq_new(2, arena);
    إذا (!ارزښتونه) {
        أرجع عدم;
    }
    asdl_seq_سېټ(ارزښتونه, 0, debug_متن);
    asdl_seq_سېټ(ارزښتونه, 1, formatted_ارزښت);
    أرجع _PyAST_JoinedStr(ارزښتونه, lineno, ستون_دننه_واټن, debug_پای_ټکی_کرښه, debug_پای_ټکی_دننه_واټن, p->arena);
}

ساكن بیان_ty
build_concatenated_بایټونه(Parser *p, asdl_بیان_seq *مزي_ګانې, صحيح lineno,
                        صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(مزي_ګانې);
    أكد(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *ډول = asdl_seq_GET(مزي_ګانې, 0)->v.Constant.ډول;

    Py_ssize_t ټول = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty elem = asdl_seq_GET(مزي_ګانې, i);
        PyObject *بایټونه = elem->v.Constant.ارزښت;
        Py_ssize_t part = Py_بایټونه_GET_اندازه(بایټونه);
        إذا (part > PY_SSIZE_T_MAX - ټول) {
            Py_Err_No_حافظه();
            أرجع عدم;
        }
        ټول += part;
    }

    Py_بایټونه_Writer *writer = Py_بایټونه_Writer_Create(ټول);
    إذا (writer == عدم) {
        أرجع عدم;
    }
    حرف *بهر = Py_بایټونه_Writer_Get_معلومات(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty elem = asdl_seq_GET(مزي_ګانې, i);
        PyObject *بایټونه = elem->v.Constant.ارزښت;
        Py_ssize_t part = Py_بایټونه_GET_اندازه(بایټونه);
        إذا (part > 0) {
            حافظه_نقل(بهر, Py_بایټونه_AS_STRING(بایټونه), part);
            بهر += part;
        }
    }

    PyObject *res = Py_بایټونه_Writer_پای(writer);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(res, ډول, lineno, ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, p->arena);
}

ساكن بیان_ty
_build_concatenated_unicode(Parser *p, asdl_بیان_seq *مزي_ګانې, صحيح lineno,
                        صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(مزي_ګانې);
    أكد(len > 1);

    بیان_ty لومړی = asdl_seq_GET(مزي_ګانې, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *ډول = لومړی->v.Constant.ډول;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    إذا (writer == عدم) {
        أرجع عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty current_elem = asdl_seq_GET(مزي_ګانې, i);
        أكد(current_elem->ډول == Constant_ډول);

        إذا (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.ارزښت)) {
            PyUnicodeWriter_Discard(writer);
            أرجع عدم;
        }
    }

    PyObject *final = Py_Unicode_Writer_پای(writer);
    إذا (final == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(final, ډول, lineno, ستون_دننه_واټن,
                           پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
}

ساكن asdl_بیان_seq *
_build_concatenated_str(Parser *p, asdl_بیان_seq *مزي_ګانې,
                               صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                               صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(مزي_ګانې);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty elem = asdl_seq_GET(مزي_ګانې, i);
        بدل(elem->ډول) {
            حالة Joined_Str_ډول:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.ارزښتونه);
                اكسر;
            حالة Template_Str_ډول:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.ارزښتونه);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_بیان_seq* flattened = Py_asdl_بیان_seq_new(n_flattened_elements, p->arena);
    إذا (flattened == عدم) {
        أرجع عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        بیان_ty elem = asdl_seq_GET(مزي_ګانې, i);
        بدل(elem->ډول) {
            حالة Joined_Str_ډول:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.ارزښتونه); j++) {
                    بیان_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.ارزښتونه, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_سېټ(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة Template_Str_ډول:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.ارزښتونه); j++) {
                    بیان_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.ارزښتونه, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_سېټ(flattened, current_pos++, subvalue);
                }
                اكسر;
            افتراضي:
                asdl_seq_سېټ(flattened, current_pos++, elem);
                اكسر;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحيح prev_is_constant = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        بیان_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        إذا (elem->ډول == Constant_ډول &&
            PyUnicode_CheckExact(elem->v.Constant.ارزښت) &&
            Py_Unicode_GET_اوږدتیا(elem->v.Constant.ارزښت) == 0)
            استمر;

        إذا (!prev_is_constant || elem->ډول != Constant_ډول) {
            n_elements++;
        }
        prev_is_constant = elem->ډول == Constant_ډول;
    }

    asdl_بیان_seq* ارزښتونه = Py_asdl_بیان_seq_new(n_elements, p->arena);
    إذا (ارزښتونه == عدم) {
        أرجع عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        بیان_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        إذا (elem->ډول == Constant_ډول) {
            إذا (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->ډول == Constant_ډول) {
                بیان_ty لومړی_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *ډول = elem->v.Constant.ډول;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                إذا (writer == عدم) {
                    أرجع عدم;
                }
                بیان_ty وروستی_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    بیان_ty current_elem = asdl_seq_GET(flattened, j);
                    إذا (current_elem->ډول == Constant_ډول) {
                        إذا (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.ارزښت)) {
                            PyUnicodeWriter_Discard(writer);
                            أرجع عدم;
                        }
                        وروستی_elem = current_elem;
                    } وإلا {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_پای(writer);
                إذا (concat_str == عدم) {
                    أرجع عدم;
                }
                إذا (Py_Arena_جمع_کړه_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    أرجع عدم;
                }
                elem = _PyAST_Constant(concat_str, ډول, لومړی_elem->lineno,
                                       لومړی_elem->ستون_دننه_واټن,
                                       وروستی_elem->پای_ټکی_lineno,
                                       وروستی_elem->پای_ټکی_ستون_دننه_واټن, p->arena);
                إذا (elem == عدم) {
                    أرجع عدم;
                }
            }

            /* Drop all empty contanst strings */
            إذا (PyUnicode_CheckExact(elem->v.Constant.ارزښت) &&
                Py_Unicode_GET_اوږدتیا(elem->v.Constant.ارزښت) == 0) {
                استمر;
            }
        }

        asdl_seq_سېټ(ارزښتونه, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    أرجع ارزښتونه;
}

ساكن بیان_ty
_build_concatenated_joined_str(Parser *p, asdl_بیان_seq *مزي_ګانې,
                               صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                               صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena)
{
    asdl_بیان_seq *ارزښتونه = _build_concatenated_str(p, مزي_ګانې, lineno,
        ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
    إذا (!ارزښتونه) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(ارزښتونه, lineno, ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, p->arena);
}

بیان_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_بیان_seq *مزي_ګانې,
                               صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                               صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena)
{
    asdl_بیان_seq *ارزښتونه = _build_concatenated_str(p, مزي_ګانې, lineno,
        ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
    إذا (!ارزښتونه) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(ارزښتونه, lineno, ستون_دننه_واټن, پای_ټکی_lineno,
        پای_ټکی_ستون_دننه_واټن, arena);
}

بیان_ty
Py_Pegen_concatenate_مزي_ګانې(Parser *p, asdl_بیان_seq *مزي_ګانې,
                             صحيح lineno, صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno,
                             صحيح پای_ټکی_ستون_دننه_واټن, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(مزي_ګانې);
    أكد(len > 0);

    صحيح f_string_وموندل = 0;
    صحيح unicode_string_وموندل = 0;
    صحيح بایټونه_وموندل = 0;

    Py_ssize_t i = 0;
    لكل (i = 0; i < len; i++) {
        بیان_ty elem = asdl_seq_GET(مزي_ګانې, i);
        بدل(elem->ډول) {
            حالة Constant_ډول:
                إذا (Py_بایټونه_Check_Exact(elem->v.Constant.ارزښت)) {
                    بایټونه_وموندل = 1;
                } وإلا {
                    unicode_string_وموندل = 1;
                }
                اكسر;
            حالة Joined_Str_ډول:
                f_string_وموندل = 1;
                اكسر;
            حالة Template_Str_ډول:
                // python.gram handles this; we should never get here
                أكد(0);
                اكسر;
            افتراضي:
                f_string_وموندل = 1;
                اكسر;
        }
    }

    // Cannot mix unicode and bytes
    إذا ((unicode_string_وموندل || f_string_وموندل) && بایټونه_وموندل) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        أرجع عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    إذا (!f_string_وموندل) {
        إذا (len == 1) {
            أرجع asdl_seq_GET(مزي_ګانې, 0);
        }
        وإلا إذا (بایټونه_وموندل) {
            أرجع build_concatenated_بایټونه(p, مزي_ګانې, lineno, ستون_دننه_واټن,
                پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
        }
        وإلا {
            أرجع _build_concatenated_unicode(p, مزي_ګانې, lineno, ستون_دننه_واټن,
                پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
        }
    }

    أرجع _build_concatenated_joined_str(p, مزي_ګانې, lineno,
        ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
}

ساكن صحيح
warn_relative_import_د_lazy(Parser *p, asdl_seq *dots, بیان_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    إذا (p->call_invalid_قواعد) {
        أرجع 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *وروستی_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    إذا (
        وروستی_dot->پای_ټکی_lineno == module->lineno
        && وروستی_dot->پای_ټکی_ستون_دننه_واټن == module->ستون_دننه_واټن
    ) {
        أرجع 0;
    }

    صحيح شمېر = Py_Pegen_seq_شمېر_dots(dots);
    حرف *buf = PyMem_RawMalloc(شمېر + 1);
    إذا (buf == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    memset(buf, '.', شمېر);
    buf[شمېر] = '\0';

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
                                       module->ستون_دننه_واټن + 1,
                                       module->پای_ټکی_lineno,
                                       module->پای_ټکی_ستون_دننه_واټن + 1,
                                       info.module);
    Py_DECREF(پیغام);
    أرجع res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, بیان_ty module_name,
                             asdl_alias_seq *نومونه, بیان_ty lazy_token, صحيح lineno,
                             صحيح ستون_دننه_واټن, صحيح پای_ټکی_lineno, صحيح پای_ټکی_ستون_دننه_واټن,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = Py_Pegen_seq_شمېر_dots(dots);
    إذا (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        إذا (lazy_token) {
            RAISE_SYNTAX_ERROR_معلوم_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < asdl_seq_LEN(نومونه); i++) {
            alias_ty alias = asdl_seq_GET(نومونه, i);
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
        إذا (warn_relative_import_د_lazy(p, dots, module_name) < 0) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_ImportFrom(module, نومونه, level, lazy_token ? 1 : 0, lineno,
                             ستون_دننه_واټن, پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, arena);
}

asdl_stmt_seq*
Py_Pegen_ثبتوونکی_stmts(Parser *p, asdl_stmt_seq* stmts) {
    إذا (!p->call_invalid_قواعد) {
        أرجع stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    إذا (len == 0) {
        أرجع stmts;
    }
    stmt_ty وروستی_stmt = asdl_seq_GET(stmts, len - 1);
    إذا (p->وروستی_stmt_location.lineno > وروستی_stmt->lineno) {
        أرجع stmts;
    }
    p->وروستی_stmt_location.lineno = وروستی_stmt->lineno;
    p->وروستی_stmt_location.ستون_دننه_واټن = وروستی_stmt->ستون_دننه_واټن;
    p->وروستی_stmt_location.پای_ټکی_lineno = وروستی_stmt->پای_ټکی_lineno;
    p->وروستی_stmt_location.پای_ټکی_ستون_دننه_واټن = وروستی_stmt->پای_ټکی_ستون_دننه_واټن;
    أرجع stmts;
}
