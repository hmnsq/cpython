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
    asdl_seq_مجموعه_UNTYPED(seq, 0, a);
    أرجع seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_درج_درون_front(Parser *p, فراغ *a, asdl_seq *seq)
{
    أكد(a != عدم);
    إذا (!seq) {
        أرجع _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    asdl_seq_مجموعه_UNTYPED(new_seq, 0, a);
    لكل (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_مجموعه_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    أرجع new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_مقصد_انتها(Parser *p, asdl_seq *seq, فراغ *a)
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
        asdl_seq_مجموعه_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_مجموعه_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
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
            asdl_seq_مجموعه_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    أكد(flattened_seq_idx == flattened_seq_اندازه);

    أرجع flattened_seq;
}

فراغ *
Py_Pegen_seq_آخرین_قلم(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أرجع asdl_seq_GET_UNTYPED(seq, len - 1);
}

فراغ *
Py_Pegen_seq_اولی_قلم(asdl_seq *seq)
{
    أرجع asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
عبارت_ty
Py_Pegen_join_نامها_with_dot(Parser *p, عبارت_ty اولی_name, عبارت_ty دومی_name)
{
    أكد(اولی_name != عدم && دومی_name != عدم);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            اولی_name->v.Name.id, دومی_name->v.Name.id);
    إذا (!uni) {
        أرجع عدم;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        أرجع عدم;
    }

    أرجع _PyAST_Name(uni, بارگیری, EXTRA_عبارت(اولی_name, دومی_name));
}

/* Counts the total number of dots in seq's tokens */
صحيح
Py_Pegen_seq_شمار_dots(asdl_seq *seq)
{
    صحيح عدد_از_dots = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_عبارت = asdl_seq_GET_UNTYPED(seq, i);
        بدل (current_عبارت->type) {
            حالة ELLIPSIS:
                عدد_از_dots += 3;
                اكسر;
            حالة DOT:
                عدد_از_dots += 1;
                اكسر;
            افتراضي:
                Py_UNREACHABLE();
        }
    }

    أرجع عدد_از_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_ستاره(Parser *p, صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                        صحيح انتها_ستون_جابه_جایی, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    إذا (!str) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_alias(str, عدم, lineno, ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_نامها_مقصد_ids(Parser *p, asdl_عبارت_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty e = asdl_seq_GET(seq, i);
        asdl_seq_مجموعه(new_seq, i, e->v.Name.id);
    }
    أرجع new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_عبارت_Pair *
Py_Pegen_cmpop_عبارت_pair(Parser *p, cmpop_ty cmpop, عبارت_ty عبارت)
{
    أكد(عبارت != عدم);
    Cmpop_عبارت_Pair *a = _PyArena_Malloc(p->arena, حجم(Cmpop_عبارت_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->cmpop = cmpop;
    a->عبارت = عبارت;
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
        Cmpop_عبارت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->cmpop);
    }
    أرجع new_seq;
}

asdl_عبارت_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    أكد(len > 0);

    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_عبارت_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->عبارت);
    }
    أرجع new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
ساكن asdl_عبارت_seq *
مجموعه_seq_context(Parser *p, asdl_عبارت_seq *seq, عبارت_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    إذا (len == 0) {
        أرجع عدم;
    }

    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty e = asdl_seq_GET(seq, i);
        عبارت_ty new_e = Py_Pegen_مجموعه_عبارت_context(p, e, ctx);
        إذا (!new_e) {
            أرجع عدم;
        }
        asdl_seq_مجموعه(new_seq, i, new_e);
    }
    أرجع new_seq;
}

ساكن عبارت_ty
مجموعه_name_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    أرجع _PyAST_Name(e->v.Name.id, ctx, EXTRA_عبارت(e, e));
}

ساكن عبارت_ty
مجموعه_tuple_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    asdl_عبارت_seq *seq = مجموعه_seq_context(p, e->v.Tuple.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_Tuple(seq, ctx, EXTRA_عبارت(e, e));
}

ساكن عبارت_ty
مجموعه_list_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    asdl_عبارت_seq *seq = مجموعه_seq_context(p, e->v.List.elts, ctx);
    إذا (!seq && PyErr_Occurred()) {
        أرجع عدم;
    }
    أرجع _PyAST_List(seq, ctx, EXTRA_عبارت(e, e));
}

ساكن عبارت_ty
مجموعه_subscript_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    أرجع _PyAST_Subscript(e->v.Subscript.مقدار, e->v.Subscript.برش,
                            ctx, EXTRA_عبارت(e, e));
}

ساكن عبارت_ty
مجموعه_attribute_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    أرجع _PyAST_Attribute(e->v.Attribute.مقدار, e->v.Attribute.attr,
                            ctx, EXTRA_عبارت(e, e));
}

ساكن عبارت_ty
مجموعه_starred_context(Parser *p, عبارت_ty e, عبارت_context_ty ctx)
{
    عبارت_ty inner = Py_Pegen_مجموعه_عبارت_context(p, e->v.Starred.مقدار, ctx);
    إذا (!inner) {
        أرجع عدم;
    }
    أرجع _PyAST_Starred(inner, ctx, EXTRA_عبارت(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
عبارت_ty
Py_Pegen_مجموعه_عبارت_context(Parser *p, عبارت_ty عبارت, عبارت_context_ty ctx)
{
    أكد(عبارت != عدم);

    عبارت_ty new = عدم;
    بدل (عبارت->نوع) {
        حالة Name_نوع:
            new = مجموعه_name_context(p, عبارت, ctx);
            اكسر;
        حالة Tuple_نوع:
            new = مجموعه_tuple_context(p, عبارت, ctx);
            اكسر;
        حالة List_نوع:
            new = مجموعه_list_context(p, عبارت, ctx);
            اكسر;
        حالة Subscript_نوع:
            new = مجموعه_subscript_context(p, عبارت, ctx);
            اكسر;
        حالة Attribute_نوع:
            new = مجموعه_attribute_context(p, عبارت, ctx);
            اكسر;
        حالة Starred_نوع:
            new = مجموعه_starred_context(p, عبارت, ctx);
            اكسر;
        افتراضي:
            new = عبارت;
    }
    أرجع new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
عبارت_ty
Py_Pegen_make_هدف(Parser *p, عبارت_ty عبارت, TARGETS_TYPE targets_type)
{
    أكد(عبارت != عدم);
    أكد(targets_type != FOR_TARGETS);
    إذا (Py_Pegen_get_invalid_هدف(عبارت, targets_type) != عدم) {
        أرجع عدم;
    }
    أرجع Py_Pegen_مجموعه_عبارت_context(
        p, عبارت, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
کلید_مقدار_Pair *
Py_Pegen_کلید_مقدار_pair(Parser *p, عبارت_ty کلید, عبارت_ty مقدار)
{
    کلید_مقدار_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_مقدار_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلید = کلید;
    a->مقدار = مقدار;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_عبارت_seq *
Py_Pegen_get_کلیدها(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_مقدار_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->کلید);
    }
    أرجع new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_عبارت_seq *
Py_Pegen_get_مقادیر(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_مقدار_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->مقدار);
    }
    أرجع new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
کلید_الگو_Pair *
Py_Pegen_کلید_الگو_pair(Parser *p, عبارت_ty کلید, الگو_ty الگو)
{
    کلید_الگو_Pair *a = _PyArena_Malloc(p->arena, حجم(کلید_الگو_Pair));
    إذا (!a) {
        أرجع عدم;
    }
    a->کلید = کلید;
    a->الگو = الگو;
    أرجع a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_عبارت_seq *
Py_Pegen_get_الگو_کلیدها(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_الگو_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->کلید);
    }
    أرجع new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_الگو_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_الگو_seq *new_seq = Py_asdl_الگو_seq_new(len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        کلید_الگو_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_مجموعه(new_seq, i, pair->الگو);
    }
    أرجع new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, عبارت_ty مقدار, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, حجم(NameDefaultPair));
    إذا (!a) {
        أرجع عدم;
    }
    a->arg = Py_Pegen_جمع_کن_type_comment_مقصد_arg(p, arg, tc);
    إذا (!a->arg) {
        أرجع عدم;
    }
    a->مقدار = مقدار;
    أرجع a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_نامها, asdl_seq *نامها_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, حجم(SlashWithDefault));
    إذا (!a) {
        أرجع عدم;
    }
    a->plain_نامها = plain_نامها;
    a->نامها_with_defaults = نامها_with_defaults;
    أرجع a;
}

/* Constructs a StarEtc */
ستاره_Etc *
Py_Pegen_ستاره_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    ستاره_Etc *a = _PyArena_Malloc(p->arena, حجم(ستاره_Etc));
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
    Py_ssize_t اولی_len = asdl_seq_LEN(a);
    Py_ssize_t دومی_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(اولی_len + دومی_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح k = 0;
    لكل (Py_ssize_t i = 0; i < اولی_len; i++) {
        asdl_seq_مجموعه_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    لكل (Py_ssize_t i = 0; i < دومی_len; i++) {
        asdl_seq_مجموعه_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    أرجع new_seq;
}

ساكن asdl_arg_seq*
get_نامها(Parser *p, asdl_seq *نامها_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نامها_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نامها_with_defaults, i);
        asdl_seq_مجموعه(seq, i, pair->arg);
    }
    أرجع seq;
}

ساكن asdl_عبارت_seq *
_get_defaults(Parser *p, asdl_seq *نامها_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(نامها_with_defaults);
    asdl_عبارت_seq *seq = Py_asdl_عبارت_seq_new(len, p->arena);
    إذا (!seq) {
        أرجع عدم;
    }
    لكل (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(نامها_with_defaults, i);
        asdl_seq_مجموعه(seq, i, pair->مقدار);
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
        asdl_arg_seq *slash_with_default_نامها =
                get_نامها(p, slash_with_default->نامها_with_defaults);
        إذا (!slash_with_default_نامها) {
            أرجع -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_نامها,
                (asdl_seq*)slash_with_default_نامها);
    }
    وإلا {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    أرجع *posonlyargs == عدم ? -1 : 0;
}

ساكن صحيح
_make_posargs(Parser *p,
              asdl_arg_seq *plain_نامها,
              asdl_seq *نامها_with_default,
              asdl_arg_seq **posargs) {

    إذا (نامها_with_default != عدم) {
        إذا (plain_نامها != عدم) {
            asdl_arg_seq *نامها_with_default_نامها = get_نامها(p, نامها_with_default);
            إذا (!نامها_with_default_نامها) {
                أرجع -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_نامها, (asdl_seq*)نامها_with_default_نامها);
        }
        وإلا {
            *posargs = get_نامها(p, نامها_with_default);
        }
    }
    وإلا {
        إذا (plain_نامها != عدم) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            أكد(0);
            *posargs = plain_نامها;
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
                  asdl_seq *نامها_with_default,
                  asdl_عبارت_seq **posdefaults) {
    إذا (slash_with_default != عدم && نامها_with_default != عدم) {
        asdl_عبارت_seq *slash_with_default_مقادیر =
                _get_defaults(p, slash_with_default->نامها_with_defaults);
        إذا (!slash_with_default_مقادیر) {
            أرجع -1;
        }
        asdl_عبارت_seq *نامها_with_default_مقادیر = _get_defaults(p, نامها_with_default);
        إذا (!نامها_with_default_مقادیر) {
            أرجع -1;
        }
        *posdefaults = (asdl_عبارت_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_مقادیر,
                (asdl_seq*)نامها_with_default_مقادیر);
    }
    وإلا إذا (slash_with_default == عدم && نامها_with_default != عدم) {
        *posdefaults = _get_defaults(p, نامها_with_default);
    }
    وإلا إذا (slash_with_default != عدم && نامها_with_default == عدم) {
        *posdefaults = _get_defaults(p, slash_with_default->نامها_with_defaults);
    }
    وإلا {
        *posdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    }
    أرجع *posdefaults == عدم ? -1 : 0;
}

ساكن صحيح
_make_kwargs(Parser *p, ستاره_Etc *ستاره_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_عبارت_seq **kwdefaults) {
    إذا (ستاره_etc != عدم && ستاره_etc->kwonlyargs != عدم) {
        *kwonlyargs = get_نامها(p, ستاره_etc->kwonlyargs);
    }
    وإلا {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    إذا (*kwonlyargs == عدم) {
        أرجع -1;
    }

    إذا (ستاره_etc != عدم && ستاره_etc->kwonlyargs != عدم) {
        *kwdefaults = _get_defaults(p, ستاره_etc->kwonlyargs);
    }
    وإلا {
        *kwdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    }

    إذا (*kwdefaults == عدم) {
        أرجع -1;
    }

    أرجع 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_نامها,
                        asdl_seq *نامها_with_default, ستاره_Etc *ستاره_etc)
{
    asdl_arg_seq *posonlyargs;
    إذا (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        أرجع عدم;
    }

    asdl_arg_seq *posargs;
    إذا (_make_posargs(p, plain_نامها, نامها_with_default, &posargs) == -1) {
        أرجع عدم;
    }

    asdl_عبارت_seq *posdefaults;
    إذا (_make_posdefaults(p,slash_with_default, نامها_with_default, &posdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty vararg = عدم;
    إذا (ستاره_etc != عدم && ستاره_etc->vararg != عدم) {
        vararg = ستاره_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_عبارت_seq *kwdefaults;
    إذا (_make_kwargs(p, ستاره_etc, &kwonlyargs, &kwdefaults) == -1) {
        أرجع عدم;
    }

    arg_ty kwarg = عدم;
    إذا (ستاره_etc != عدم && ستاره_etc->kwarg != عدم) {
        kwarg = ستاره_etc->kwarg;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_خالی_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posonlyargs) {
        أرجع عدم;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!posargs) {
        أرجع عدم;
    }
    asdl_عبارت_seq *posdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    إذا (!posdefaults) {
        أرجع عدم;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    إذا (!kwonlyargs) {
        أرجع عدم;
    }
    asdl_عبارت_seq *kwdefaults = Py_asdl_عبارت_seq_new(0, p->arena);
    إذا (!kwdefaults) {
        أرجع عدم;
    }

    أرجع _PyAST_arguments(posonlyargs, posargs, عدم, kwonlyargs,
                            kwdefaults, عدم, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty نوع)
{
    AugOperator *a = _PyArena_Malloc(p->arena, حجم(AugOperator));
    إذا (!a) {
        أرجع عدم;
    }
    a->نوع = نوع;
    أرجع a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_عبارت_seq *decorators, stmt_ty function_def)
{
    أكد(function_def != عدم);
    إذا (function_def->نوع == Async_Function_Def_نوع) {
        أرجع _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.آرگومانها,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->ستون_جابه_جایی,
            function_def->انتها_lineno, function_def->انتها_ستون_جابه_جایی, p->arena);
    }

    أرجع _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.آرگومانها,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->ستون_جابه_جایی,
        function_def->انتها_lineno, function_def->انتها_ستون_جابه_جایی, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_عبارت_seq *decorators, stmt_ty class_def)
{
    أكد(class_def != عدم);
    أرجع _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->ستون_جابه_جایی, class_def->انتها_lineno,
        class_def->انتها_ستون_جابه_جایی, p->arena);
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
seq_عدد_از_starred_exprs(asdl_seq *seq)
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
asdl_عبارت_seq *
Py_Pegen_seq_استخراج_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    صحيح new_len = seq_عدد_از_starred_exprs(kwargs);
    إذا (new_len == 0) {
        أرجع عدم;
    }
    asdl_عبارت_seq *new_seq = Py_asdl_عبارت_seq_new(new_len, p->arena);
    إذا (!new_seq) {
        أرجع عدم;
    }

    صحيح idx = 0;
    لكل (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        إذا (!k->is_keyword) {
            asdl_seq_مجموعه(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_عدد_از_starred_exprs(kwargs);
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
            asdl_seq_مجموعه(new_seq, idx++, k->element);
        }
    }
    أرجع new_seq;
}

عبارت_ty
_PyPegen_ensure_imaginary(Parser *p, عبارت_ty نمایی_تابع)
{
    إذا (نمایی_تابع->نوع != Constant_نوع || !PyComplex_CheckExact(نمایی_تابع->v.Constant.مقدار)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(نمایی_تابع, "imaginary number required in complex literal");
        أرجع عدم;
    }
    أرجع نمایی_تابع;
}

عبارت_ty
_PyPegen_ensure_real(Parser *p, عبارت_ty نمایی_تابع)
{
    إذا (نمایی_تابع->نوع != Constant_نوع || PyComplex_CheckExact(نمایی_تابع->v.Constant.مقدار)) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(نمایی_تابع, "real number required in complex literal");
        أرجع عدم;
    }
    أرجع نمایی_تابع;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = عدم;
    Py_ssize_t رقم = p->type_ignore_comments.رقم_اقلام;
    إذا (رقم > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(رقم, p->arena);
        إذا (type_ignores == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < رقم; i++) {
            PyObject *برچسب = _PyPegen_new_type_comment(p, p->type_ignore_comments.اقلام[i].comment);
            إذا (برچسب == عدم) {
                أرجع عدم;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.اقلام[i].lineno,
                                                  برچسب, p->arena);
            إذا (ti == عدم) {
                أرجع عدم;
            }
            asdl_seq_مجموعه(type_ignores, i, ti);
        }
    }
    أرجع _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ثابت حرف *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, طول_رشته(s), عدم);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع res;
}

arg_ty
Py_Pegen_جمع_کن_type_comment_مقصد_arg(Parser *p, arg_ty a, Token *tc)
{
    إذا (tc == عدم) {
        أرجع a;
    }
    ثابت حرف *بایتها = Py_بایتها_As_String(tc->بایتها);
    إذا (بایتها == عدم) {
        أرجع عدم;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, بایتها);
    إذا (tco == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->ستون_جابه_جایی, a->انتها_lineno, a->انتها_ستون_جابه_جایی,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
صحيح
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    أكد(t->بایتها != عدم);
    أكد(t->type == NOTEQUAL);

    ثابت حرف* tok_str = Py_بایتها_AS_STRING(t->بایتها);
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
_PyPegen_check_legacy_stmt(Parser *p, عبارت_ty name) {
    إذا (name->نوع != Name_نوع) {
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
_PyPegen_raise_error_for_missing_comma(Parser *p, عبارت_ty a, عبارت_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    إذا (_PyPegen_check_legacy_stmt(p, a)) {
        أرجع عدم;
    }
    // Only raise inside parentheses/brackets (level > 0)
    إذا (p->نشانهها[p->mark - 1]->level == 0) {
        أرجع عدم;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    إذا (a->انتها_lineno > a->lineno) {
        أرجع RAISE_ERROR_معلوم_LOCATION(
            p, PyExc_SyntaxError, a->انتها_lineno, a->ستون_جابه_جایی,
            a->انتها_lineno, a->انتها_ستون_جابه_جایی,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    أرجع RAISE_ERROR_معلوم_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->ستون_جابه_جایی,
        b->انتها_lineno, b->انتها_ستون_جابه_جایی,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

ساكن نتیجه_Token_With_Metadata *
نتیجه_token_with_metadata(Parser *p, فراغ *نتیجه, PyObject *metadata)
{
    نتیجه_Token_With_Metadata *res = _PyArena_Malloc(p->arena, حجم(نتیجه_Token_With_Metadata));
    إذا (res == عدم) {
        أرجع عدم;
    }
    res->metadata = metadata;
    res->نتیجه = نتیجه;
    أرجع res;
}

ساكن حرف
formatted_string_پیشوند(ثابت Parser *p)
{
    صحيح nested = 0;
    لكل (صحيح i = p->mark - 1; i >= 0; i--) {
        صحيح type = p->نشانهها[i]->type;
        إذا (type == FSTRING_انتها || type == TSTRING_انتها) {
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

نتیجه_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, عبارت_ty conv)
{
    إذا (conv_token->lineno != conv->lineno || conv_token->انتها_ستون_جابه_جایی != conv->ستون_جابه_جایی) {
        أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_پیشوند(p)
        );
    }

    Py_UCS4 اولی = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    إذا (Py_Unicode_GET_درازی(conv->v.Name.id) > 1 ||
            !(اولی == 's' || اولی == 'r' || اولی == 'a')) {
        RAISE_SYNTAX_ERROR_معلوم_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_پیشوند(p),
                                            conv->v.Name.id);
        أرجع عدم;
    }

    أرجع نتیجه_token_with_metadata(p, conv, conv_token->metadata);
}

نتیجه_Token_With_Metadata *
Py_Pegen_راهاندازی_پر_format_spec(Parser *p, Token *colon, asdl_عبارت_seq *spec, صحيح lineno, صحيح ستون_جابه_جایی,
                                صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی, PyArena *arena)
{
    إذا (!spec) {
        أرجع عدم;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_اقلام = asdl_seq_LEN(spec);
    Py_ssize_t non_خالی_شمار = 0;
    لكل (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty قلم = asdl_seq_GET(spec, i);
        non_خالی_شمار += !(قلم->نوع == Constant_نوع &&
                             PyUnicode_CheckExact(قلم->v.Constant.مقدار) &&
                             Py_Unicode_GET_درازی(قلم->v.Constant.مقدار) == 0);
    }
    إذا (non_خالی_شمار != n_اقلام) {
        asdl_عبارت_seq *resized_spec =
            Py_asdl_عبارت_seq_new(non_خالی_شمار, p->arena);
        إذا (resized_spec == عدم) {
            أرجع عدم;
        }
        Py_ssize_t j = 0;
        لكل (Py_ssize_t i = 0; i < n_اقلام; i++) {
            عبارت_ty قلم = asdl_seq_GET(spec, i);
            إذا (قلم->نوع == Constant_نوع &&
                PyUnicode_CheckExact(قلم->v.Constant.مقدار) &&
                Py_Unicode_GET_درازی(قلم->v.Constant.مقدار) == 0) {
                استمر;
            }
            asdl_seq_مجموعه(resized_spec, j++, قلم);
        }
        أكد(j == non_خالی_شمار);
        spec = resized_spec;
    }
    عبارت_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    إذا (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->نوع == Constant_نوع)) {
        res = _PyAST_JoinedStr(spec, lineno, ستون_جابه_جایی, انتها_lineno,
                                    انتها_ستون_جابه_جایی, p->arena);
    } وإلا {
        res = Py_Pegen_concatenate_رشتهها(p, spec,
                             lineno, ستون_جابه_جایی, انتها_lineno,
                             انتها_ستون_جابه_جایی, arena);
    }
    إذا (!res) {
        أرجع عدم;
    }
    أرجع نتیجه_token_with_metadata(p, res, colon->metadata);
}

ثابت حرف *
Py_Pegen_get_عبارت_name(عبارت_ty e)
{
    أكد(e != عدم);
    بدل (e->نوع) {
        حالة Attribute_نوع:
            أرجع "attribute";
        حالة Subscript_نوع:
            أرجع "subscript";
        حالة Starred_نوع:
            أرجع "starred";
        حالة Name_نوع:
            أرجع "name";
        حالة List_نوع:
            أرجع "list";
        حالة Tuple_نوع:
            أرجع "tuple";
        حالة Lambda_نوع:
            أرجع "lambda";
        حالة Call_نوع:
            أرجع "function call";
        حالة Bool_Op_نوع:
        حالة Bin_Op_نوع:
        حالة Unary_Op_نوع:
            أرجع "expression";
        حالة Generator_Exp_نوع:
            أرجع "generator expression";
        حالة Yield_نوع:
        حالة Yield_From_نوع:
            أرجع "yield expression";
        حالة Await_نوع:
            أرجع "await expression";
        حالة List_Comp_نوع:
            أرجع "list comprehension";
        حالة مجموعه_Comp_نوع:
            أرجع "set comprehension";
        حالة Dict_Comp_نوع:
            أرجع "dict comprehension";
        حالة Dict_نوع:
            أرجع "dict literal";
        حالة مجموعه_نوع:
            أرجع "set display";
        حالة Joined_Str_نوع:
        حالة Formatted_مقدار_نوع:
            أرجع "f-string expression";
        حالة Template_Str_نوع:
        حالة Interpolation_نوع:
            أرجع "t-string expression";
        حالة Constant_نوع: {
            PyObject *مقدار = e->v.Constant.مقدار;
            إذا (مقدار == Py_None) {
                أرجع "None";
            }
            إذا (مقدار == Py_False) {
                أرجع "False";
            }
            إذا (مقدار == Py_True) {
                أرجع "True";
            }
            إذا (مقدار == Py_Ellipsis) {
                أرجع "ellipsis";
            }
            أرجع "literal";
        }
        حالة Compare_نوع:
            أرجع "comparison";
        حالة If_Exp_نوع:
            أرجع "conditional expression";
        حالة Named_عبارت_نوع:
            أرجع "named expression";
        افتراضي:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->نوع, e->lineno);
            أرجع عدم;
    }
}

عبارت_ty
Py_Pegen_get_آخرین_comprehension_قلم(comprehension_ty comprehension) {
    إذا (comprehension->ifs == عدم || asdl_seq_LEN(comprehension->ifs) == 0) {
        أرجع comprehension->iter;
    }
    أرجع Py_Pegen_آخرین_قلم(comprehension->ifs, عبارت_ty);
}

عبارت_ty _PyPegen_collect_call_seqs(Parser *p, asdl_عبارت_seq *a, asdl_seq *b,
                     صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                     صحيح انتها_ستون_جابه_جایی, PyArena *arena) {
    Py_ssize_t آرگومانها_len = asdl_seq_LEN(a);
    Py_ssize_t مجموع_len = آرگومانها_len;

    إذا (b == عدم) {
        أرجع _PyAST_Call(_PyPegen_dummy_name(p), a, عدم, lineno, ستون_جابه_جایی,
                        انتها_lineno, انتها_ستون_جابه_جایی, arena);

    }

    asdl_عبارت_seq *starreds = Py_Pegen_seq_استخراج_starred_exprs(p, b);
    إذا (!starreds && PyErr_Occurred()) {
        أرجع عدم;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    إذا (!keywords && PyErr_Occurred()) {
        أرجع عدم;
    }

    إذا (starreds) {
        مجموع_len += asdl_seq_LEN(starreds);
    }

    asdl_عبارت_seq *آرگومانها = Py_asdl_عبارت_seq_new(مجموع_len, arena);
    إذا (آرگومانها == عدم) {
        أرجع عدم;
    }

    Py_ssize_t i = 0;
    لكل (i = 0; i < آرگومانها_len; i++) {
        asdl_seq_مجموعه(آرگومانها, i, asdl_seq_GET(a, i));
    }
    لكل (; i < مجموع_len; i++) {
        asdl_seq_مجموعه(آرگومانها, i, asdl_seq_GET(starreds, i - آرگومانها_len));
    }

    أرجع _PyAST_Call(_PyPegen_dummy_name(p), آرگومانها, keywords, lineno,
                       ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
}

// AST Error reporting helpers

عبارت_ty
Py_Pegen_get_invalid_هدف(عبارت_ty e, TARGETS_TYPE targets_type)
{
    إذا (e == عدم) {
        أرجع عدم;
    }

    إذا (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        إذا (targets_type == SINGLE_TARGETS && e->نوع == Name_نوع) {
            أرجع عدم;
        }
        أرجع e->نوع == Attribute_نوع || e->نوع == Subscript_نوع ? عدم : e;
    }

#تعریف_کن بازدید_CONTAINER(CONTAINER, TYPE) افعل { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        لكل (Py_ssize_t i = 0; i < len; i++) {\
            عبارت_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            عبارت_ty فرزند = Py_Pegen_get_invalid_هدف(other, targets_type);\
            إذا (فرزند != عدم) {\
                أرجع فرزند;\
            }\
        }\
    } طالما (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    بدل (e->نوع) {
        حالة List_نوع:
            بازدید_CONTAINER(e, List);
            أرجع عدم;
        حالة Tuple_نوع:
            بازدید_CONTAINER(e, Tuple);
            أرجع عدم;
        حالة Starred_نوع:
            إذا (targets_type == DEL_TARGETS) {
                أرجع e;
            }
            أرجع Py_Pegen_get_invalid_هدف(e->v.Starred.مقدار, targets_type);
        حالة Compare_نوع:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            إذا (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                إذا (cmpop == درون) {
                    أرجع Py_Pegen_get_invalid_هدف(e->v.Compare.چپ, targets_type);
                }
                أرجع عدم;
            }
            أرجع e;
        حالة Name_نوع:
        حالة Subscript_نوع:
        حالة Attribute_نوع:
            أرجع عدم;
        افتراضي:
            أرجع e;
    }
}

فراغ *_PyPegen_arguments_parsing_error(Parser *p, عبارت_ty e) {
    صحيح kwarg_unpacking = 0;
    لكل (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        إذا (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ثابت حرف *پیام = عدم;
    إذا (kwarg_unpacking) {
        پیام = "positional argument follows keyword argument unpacking";
    } وإلا {
        پیام = "positional argument follows keyword argument";
    }

    أرجع RAISE_SYNTAX_ERROR(پیام);
}

فراغ *
Py_Pegen_nonparen_genexp_درون_call(Parser *p, عبارت_ty آرگومانها, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(آرگومانها->v.Call.آرگومانها);
    إذا (len <= 1) {
        أرجع عدم;
    }

    comprehension_ty آخرین_comprehension = Py_Pegen_آخرین_قلم(comprehensions, comprehension_ty);

    أرجع RAISE_SYNTAX_ERROR_معلوم_RANGE(
        (عبارت_ty) asdl_seq_GET(آرگومانها->v.Call.آرگومانها, len - 1),
        Py_Pegen_get_آخرین_comprehension_قلم(آخرین_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

ساكن عبارت_ty
_PyPegen_decode_fstring_part(Parser* p, صحيح is_raw, عبارت_ty constant, Token* token) {
    أكد(PyUnicode_CheckExact(constant->v.Constant.مقدار));

    ثابت حرف* bstr = PyUnicode_AsUTF8(constant->v.Constant.مقدار);
    إذا (bstr == عدم) {
        أرجع عدم;
    }

    اندازه_t len;
    إذا (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } وإلا {
        len = طول_رشته(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == عدم;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    إذا (str == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, constant->lineno, constant->ستون_جابه_جایی,
                           constant->انتها_lineno, constant->انتها_ستون_جابه_جایی,
                           p->arena);
}

ساكن asdl_عبارت_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions,
                   Token *b, صحيح is_tstring)
{
    Py_ssize_t n_اقلام = asdl_seq_LEN(raw_expressions);
    Py_ssize_t مجموع_اقلام = n_اقلام;
    لكل (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty قلم = asdl_seq_GET(raw_expressions, i);
        إذا (قلم->نوع == Joined_Str_نوع) {
            مجموع_اقلام += asdl_seq_LEN(قلم->v.JoinedStr.مقادیر) - 1;
        }
    }

    ثابت حرف* quote_str = Py_بایتها_As_String(a->بایتها);
    إذا (quote_str == عدم) {
        أرجع عدم;
    }
    صحيح is_raw = strpbrk(quote_str, "rR") != عدم;

    asdl_عبارت_seq *seq = Py_asdl_عبارت_seq_new(مجموع_اقلام, p->arena);
    إذا (seq == عدم) {
        أرجع عدم;
    }

    Py_ssize_t شاخص = 0;
    لكل (Py_ssize_t i = 0; i < n_اقلام; i++) {
        عبارت_ty قلم = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        إذا (قلم->نوع == Joined_Str_نوع) {
            asdl_عبارت_seq *مقادیر = قلم->v.JoinedStr.مقادیر;
            إذا (asdl_seq_LEN(مقادیر) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             قلم->lineno);
                أرجع عدم;
            }

            عبارت_ty اولی = asdl_seq_GET(مقادیر, 0);
            أكد(اولی->نوع == Constant_نوع);
            asdl_seq_مجموعه(seq, شاخص++, اولی);

            عبارت_ty دومی = asdl_seq_GET(مقادیر, 1);
            أكد((is_tstring &&
                    دومی->نوع == Interpolation_نوع) ||
                   دومی->نوع == Formatted_مقدار_نوع);
            asdl_seq_مجموعه(seq, شاخص++, دومی);

            استمر;
        }

        إذا (قلم->نوع == Constant_نوع) {
            قلم = _PyPegen_decode_fstring_part(p, is_raw, قلم, b);
            إذا (قلم == عدم) {
                أرجع عدم;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            إذا (PyUnicode_CheckExact(قلم->v.Constant.مقدار)
                && Py_Unicode_GET_درازی(قلم->v.Constant.مقدار) == 0) {
                استمر;
            }
        }
        asdl_seq_مجموعه(seq, شاخص++, قلم);
    }

    asdl_عبارت_seq *resized_exprs;
    إذا (شاخص != مجموع_اقلام) {
        resized_exprs = Py_asdl_عبارت_seq_new(شاخص, p->arena);
        إذا (resized_exprs == عدم) {
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < شاخص; i++) {
            asdl_seq_مجموعه(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    وإلا {
        resized_exprs = seq;
    }
    أرجع resized_exprs;
}

عبارت_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_عبارت_seq *raw_expressions, Token *b) {

    asdl_عبارت_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(resized_exprs, a->lineno, a->ستون_جابه_جایی,
                              b->انتها_lineno, b->انتها_ستون_جابه_جایی,
                              p->arena);
}

عبارت_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_عبارت_seq* raw_expressions, Token*b) {

    asdl_عبارت_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    إذا (resized_exprs == عدم) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(resized_exprs, a->lineno, a->ستون_جابه_جایی,
                            b->انتها_lineno, b->انتها_ستون_جابه_جایی,
                            p->arena);
}

عبارت_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بایتها == عدم) {
        أرجع Py_Pegen_raise_incomplete_درون_داد(p);
    }
    Py_ssize_t bsize = Py_بایتها_GET_اندازه(tok->بایتها);
    ثابت حرف *bstr = Py_بایتها_AS_STRING(tok->بایتها);

    صحيح is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->ستون_جابه_جایی,
                           tok->انتها_lineno, tok->انتها_ستون_جابه_جایی,
                           p->arena);
}

عبارت_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    إذا (tok->بایتها == عدم) {
        أرجع Py_Pegen_raise_incomplete_درون_داد(p);
    }
    ثابت حرف *bstr = Py_بایتها_AS_STRING(tok->بایتها);
    PyObject* str = PyUnicode_FromString(bstr);
    إذا (str == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(str, عدم, tok->lineno, tok->ستون_جابه_جایی,
                           tok->انتها_lineno, tok->انتها_ستون_جابه_جایی,
                           p->arena);
}

عبارت_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    إذا (tok->بایتها == عدم) {
        أرجع Py_Pegen_raise_incomplete_درون_داد(p);
    }
    ثابت حرف *the_str = Py_بایتها_AS_STRING(tok->بایتها);
    PyObject *s = _PyPegen_parse_string(p, tok);
    إذا (s == عدم) {
        _Pypegen_raise_decode_error(p);
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        أرجع عدم;
    }
    PyObject *نوع = عدم;
    إذا (the_str && the_str[0] == 'u') {
        نوع = _PyPegen_new_identifier(p, "u");
        إذا (نوع == عدم) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_Constant(s, نوع, tok->lineno, tok->ستون_جابه_جایی, tok->انتها_lineno, tok->انتها_ستون_جابه_جایی, p->arena);
}

ساكن صحيح
_get_interpolation_conversion(Parser *p, Token *debug, نتیجه_Token_With_Metadata *conversion,
                              نتیجه_Token_With_Metadata *format)
{
    إذا (conversion != عدم) {
        عبارت_ty conversion_عبارت = (عبارت_ty) conversion->نتیجه;
        أكد(conversion_عبارت->نوع == Name_نوع);
        Py_UCS4 اولی = PyUnicode_READ_CHAR(conversion_عبارت->v.Name.id, 0);
        أرجع Py_امن_DOWNCAST(اولی, Py_UCS4, صحيح);
    }
    وإلا إذا (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        أرجع (صحيح)'r';
    }
    أرجع -1;
}

ساكن PyObject *
strip_interpolation_debug_عبارت(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_درازی(exprstr);

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

عبارت_ty _PyPegen_interpolation(Parser *p, عبارت_ty عبارت_2, Token *debug, نتیجه_Token_With_Metadata *conversion,
                                 نتیجه_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح ستون_جابه_جایی,
                                 صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی, PyArena *arena) {

    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    صحيح debug_انتها_سطر, debug_انتها_جابه_جایی;
    PyObject *debug_metadata;
    constant exprstr;

    إذا (conversion) {
        debug_انتها_سطر = ((عبارت_ty) conversion->نتیجه)->lineno;
        debug_انتها_جابه_جایی = ((عبارت_ty) conversion->نتیجه)->ستون_جابه_جایی;
        debug_metadata = exprstr = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_انتها_سطر = ((عبارت_ty) format->نتیجه)->lineno;
        debug_انتها_جابه_جایی = ((عبارت_ty) format->نتیجه)->ستون_جابه_جایی + 1;
        debug_metadata = exprstr = format->metadata;
    }
    وإلا {
        debug_انتها_سطر = انتها_lineno;
        debug_انتها_جابه_جایی = انتها_ستون_جابه_جایی;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    أكد(exprstr != عدم);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_عبارت(exprstr)
        : Py_NewRef(exprstr);
    إذا (!final_exprstr || Py_Arena_جمع_کن_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        أرجع عدم;
    }

    عبارت_ty interpolation = _PyAST_Interpolation(
        عبارت_2, final_exprstr, conversion_val, format ? (عبارت_ty) format->نتیجه : عدم,
        lineno, ستون_جابه_جایی, انتها_lineno,
        انتها_ستون_جابه_جایی, arena
    );

    إذا (!interpolation || !debug) {
        أرجع interpolation;
    }

    عبارت_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, ستون_جابه_جایی + 1, debug_انتها_سطر,
                                            debug_انتها_جابه_جایی - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_عبارت_seq *مقادیر = Py_asdl_عبارت_seq_new(2, arena);
    إذا (!مقادیر) {
        أرجع عدم;
    }
    asdl_seq_مجموعه(مقادیر, 0, debug_متن);
    asdl_seq_مجموعه(مقادیر, 1, interpolation);
    أرجع _PyAST_JoinedStr(مقادیر, lineno, ستون_جابه_جایی, debug_انتها_سطر, debug_انتها_جابه_جایی, p->arena);
}

عبارت_ty Py_Pegen_formatted_مقدار(Parser *p, عبارت_ty عبارت_2, Token *debug, نتیجه_Token_With_Metadata *conversion,
                                 نتیجه_Token_With_Metadata *format, Token *closing_brace, صحيح lineno, صحيح ستون_جابه_جایی,
                                 صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی, PyArena *arena) {
    صحيح conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    عبارت_ty formatted_مقدار = Py_AST_Formatted_مقدار(
        عبارت_2, conversion_val, format ? (عبارت_ty) format->نتیجه : عدم,
        lineno, ستون_جابه_جایی, انتها_lineno,
        انتها_ستون_جابه_جایی, arena
    );

    إذا (!formatted_مقدار || !debug) {
        أرجع formatted_مقدار;
    }

    /* Find the non whitespace token after the "=" */
    صحيح debug_انتها_سطر, debug_انتها_جابه_جایی;
    PyObject *debug_metadata;

    إذا (conversion) {
        debug_انتها_سطر = ((عبارت_ty) conversion->نتیجه)->lineno;
        debug_انتها_جابه_جایی = ((عبارت_ty) conversion->نتیجه)->ستون_جابه_جایی;
        debug_metadata = conversion->metadata;
    }
    وإلا إذا (format) {
        debug_انتها_سطر = ((عبارت_ty) format->نتیجه)->lineno;
        debug_انتها_جابه_جایی = ((عبارت_ty) format->نتیجه)->ستون_جابه_جایی + 1;
        debug_metadata = format->metadata;
    }
    وإلا {
        debug_انتها_سطر = انتها_lineno;
        debug_انتها_جابه_جایی = انتها_ستون_جابه_جایی;
        debug_metadata = closing_brace->metadata;
    }
    عبارت_ty debug_متن = _PyAST_Constant(debug_metadata, عدم, lineno, ستون_جابه_جایی + 1, debug_انتها_سطر,
                                            debug_انتها_جابه_جایی - 1, p->arena);
    إذا (!debug_متن) {
        أرجع عدم;
    }

    asdl_عبارت_seq *مقادیر = Py_asdl_عبارت_seq_new(2, arena);
    إذا (!مقادیر) {
        أرجع عدم;
    }
    asdl_seq_مجموعه(مقادیر, 0, debug_متن);
    asdl_seq_مجموعه(مقادیر, 1, formatted_مقدار);
    أرجع _PyAST_JoinedStr(مقادیر, lineno, ستون_جابه_جایی, debug_انتها_سطر, debug_انتها_جابه_جایی, p->arena);
}

ساكن عبارت_ty
build_concatenated_بایتها(Parser *p, asdl_عبارت_seq *رشتهها, صحيح lineno,
                        صحيح ستون_جابه_جایی, صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(رشتهها);
    أكد(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *نوع = asdl_seq_GET(رشتهها, 0)->v.Constant.نوع;

    Py_ssize_t مجموع = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(رشتهها, i);
        PyObject *بایتها = elem->v.Constant.مقدار;
        Py_ssize_t part = Py_بایتها_GET_اندازه(بایتها);
        إذا (part > PY_SSIZE_T_MAX - مجموع) {
            Py_Err_No_حافظه();
            أرجع عدم;
        }
        مجموع += part;
    }

    Py_بایتها_Writer *writer = Py_بایتها_Writer_Create(مجموع);
    إذا (writer == عدم) {
        أرجع عدم;
    }
    حرف *بیرون = Py_بایتها_Writer_Get_داده(writer);

    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(رشتهها, i);
        PyObject *بایتها = elem->v.Constant.مقدار;
        Py_ssize_t part = Py_بایتها_GET_اندازه(بایتها);
        إذا (part > 0) {
            کپی_حافظه(بیرون, Py_بایتها_AS_STRING(بایتها), part);
            بیرون += part;
        }
    }

    PyObject *res = Py_بایتها_Writer_پایان(writer);
    إذا (res == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(res, نوع, lineno, ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, p->arena);
}

ساكن عبارت_ty
_build_concatenated_unicode(Parser *p, asdl_عبارت_seq *رشتهها, صحيح lineno,
                        صحيح ستون_جابه_جایی, صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(رشتهها);
    أكد(len > 1);

    عبارت_ty اولی = asdl_seq_GET(رشتهها, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *نوع = اولی->v.Constant.نوع;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    إذا (writer == عدم) {
        أرجع عدم;
    }

    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty current_elem = asdl_seq_GET(رشتهها, i);
        أكد(current_elem->نوع == Constant_نوع);

        إذا (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.مقدار)) {
            PyUnicodeWriter_Discard(writer);
            أرجع عدم;
        }
    }

    PyObject *final = Py_Unicode_Writer_پایان(writer);
    إذا (final == عدم) {
        أرجع عدم;
    }
    إذا (Py_Arena_جمع_کن_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        أرجع عدم;
    }
    أرجع _PyAST_Constant(final, نوع, lineno, ستون_جابه_جایی,
                           انتها_lineno, انتها_ستون_جابه_جایی, arena);
}

ساكن asdl_عبارت_seq *
_build_concatenated_str(Parser *p, asdl_عبارت_seq *رشتهها,
                               صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                               صحيح انتها_ستون_جابه_جایی, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(رشتهها);
    أكد(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(رشتهها, i);
        بدل(elem->نوع) {
            حالة Joined_Str_نوع:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.مقادیر);
                اكسر;
            حالة Template_Str_نوع:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.مقادیر);
                اكسر;
            افتراضي:
                n_flattened_elements++;
                اكسر;
        }
    }


    asdl_عبارت_seq* flattened = Py_asdl_عبارت_seq_new(n_flattened_elements, p->arena);
    إذا (flattened == عدم) {
        أرجع عدم;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    لكل (Py_ssize_t i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(رشتهها, i);
        بدل(elem->نوع) {
            حالة Joined_Str_نوع:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.مقادیر); j++) {
                    عبارت_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.مقادیر, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_مجموعه(flattened, current_pos++, subvalue);
                }
                اكسر;
            حالة Template_Str_نوع:
                لكل (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.مقادیر); j++) {
                    عبارت_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.مقادیر, j);
                    إذا (subvalue == عدم) {
                        أرجع عدم;
                    }
                    asdl_seq_مجموعه(flattened, current_pos++, subvalue);
                }
                اكسر;
            افتراضي:
                asdl_seq_مجموعه(flattened, current_pos++, elem);
                اكسر;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    صحيح prev_is_constant = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        عبارت_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        إذا (elem->نوع == Constant_نوع &&
            PyUnicode_CheckExact(elem->v.Constant.مقدار) &&
            Py_Unicode_GET_درازی(elem->v.Constant.مقدار) == 0)
            استمر;

        إذا (!prev_is_constant || elem->نوع != Constant_نوع) {
            n_elements++;
        }
        prev_is_constant = elem->نوع == Constant_نوع;
    }

    asdl_عبارت_seq* مقادیر = Py_asdl_عبارت_seq_new(n_elements, p->arena);
    إذا (مقادیر == عدم) {
        أرجع عدم;
    }

    /* build folded list */
    current_pos = 0;
    لكل (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        عبارت_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        إذا (elem->نوع == Constant_نوع) {
            إذا (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->نوع == Constant_نوع) {
                عبارت_ty اولی_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *نوع = elem->v.Constant.نوع;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                إذا (writer == عدم) {
                    أرجع عدم;
                }
                عبارت_ty آخرین_elem = elem;
                Py_ssize_t j;
                لكل (j = i; j < n_flattened_elements; j++) {
                    عبارت_ty current_elem = asdl_seq_GET(flattened, j);
                    إذا (current_elem->نوع == Constant_نوع) {
                        إذا (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.مقدار)) {
                            PyUnicodeWriter_Discard(writer);
                            أرجع عدم;
                        }
                        آخرین_elem = current_elem;
                    } وإلا {
                        اكسر;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_پایان(writer);
                إذا (concat_str == عدم) {
                    أرجع عدم;
                }
                إذا (Py_Arena_جمع_کن_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    أرجع عدم;
                }
                elem = _PyAST_Constant(concat_str, نوع, اولی_elem->lineno,
                                       اولی_elem->ستون_جابه_جایی,
                                       آخرین_elem->انتها_lineno,
                                       آخرین_elem->انتها_ستون_جابه_جایی, p->arena);
                إذا (elem == عدم) {
                    أرجع عدم;
                }
            }

            /* Drop all empty contanst strings */
            إذا (PyUnicode_CheckExact(elem->v.Constant.مقدار) &&
                Py_Unicode_GET_درازی(elem->v.Constant.مقدار) == 0) {
                استمر;
            }
        }

        asdl_seq_مجموعه(مقادیر, current_pos++, elem);
    }

    أكد(current_pos == n_elements);
    أرجع مقادیر;
}

ساكن عبارت_ty
_build_concatenated_joined_str(Parser *p, asdl_عبارت_seq *رشتهها,
                               صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                               صحيح انتها_ستون_جابه_جایی, PyArena *arena)
{
    asdl_عبارت_seq *مقادیر = _build_concatenated_str(p, رشتهها, lineno,
        ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
    إذا (!مقادیر) {
        أرجع عدم;
    }
    أرجع _PyAST_JoinedStr(مقادیر, lineno, ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, p->arena);
}

عبارت_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_عبارت_seq *رشتهها,
                               صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                               صحيح انتها_ستون_جابه_جایی, PyArena *arena)
{
    asdl_عبارت_seq *مقادیر = _build_concatenated_str(p, رشتهها, lineno,
        ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
    إذا (!مقادیر) {
        أرجع عدم;
    }
    أرجع _PyAST_TemplateStr(مقادیر, lineno, ستون_جابه_جایی, انتها_lineno,
        انتها_ستون_جابه_جایی, arena);
}

عبارت_ty
Py_Pegen_concatenate_رشتهها(Parser *p, asdl_عبارت_seq *رشتهها,
                             صحيح lineno, صحيح ستون_جابه_جایی, صحيح انتها_lineno,
                             صحيح انتها_ستون_جابه_جایی, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(رشتهها);
    أكد(len > 0);

    صحيح f_string_یافت = 0;
    صحيح unicode_string_یافت = 0;
    صحيح بایتها_یافت = 0;

    Py_ssize_t i = 0;
    لكل (i = 0; i < len; i++) {
        عبارت_ty elem = asdl_seq_GET(رشتهها, i);
        بدل(elem->نوع) {
            حالة Constant_نوع:
                إذا (Py_بایتها_Check_Exact(elem->v.Constant.مقدار)) {
                    بایتها_یافت = 1;
                } وإلا {
                    unicode_string_یافت = 1;
                }
                اكسر;
            حالة Joined_Str_نوع:
                f_string_یافت = 1;
                اكسر;
            حالة Template_Str_نوع:
                // python.gram handles this; we should never get here
                أكد(0);
                اكسر;
            افتراضي:
                f_string_یافت = 1;
                اكسر;
        }
    }

    // Cannot mix unicode and bytes
    إذا ((unicode_string_یافت || f_string_یافت) && بایتها_یافت) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        أرجع عدم;
    }

    // If it's only bytes or only unicode string, do a simple concat
    إذا (!f_string_یافت) {
        إذا (len == 1) {
            أرجع asdl_seq_GET(رشتهها, 0);
        }
        وإلا إذا (بایتها_یافت) {
            أرجع build_concatenated_بایتها(p, رشتهها, lineno, ستون_جابه_جایی,
                انتها_lineno, انتها_ستون_جابه_جایی, arena);
        }
        وإلا {
            أرجع _build_concatenated_unicode(p, رشتهها, lineno, ستون_جابه_جایی,
                انتها_lineno, انتها_ستون_جابه_جایی, arena);
        }
    }

    أرجع _build_concatenated_joined_str(p, رشتهها, lineno,
        ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
}

ساكن صحيح
warn_relative_import_از_lazy(Parser *p, asdl_seq *dots, عبارت_ty module)
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
    Token *آخرین_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    إذا (
        آخرین_dot->انتها_lineno == module->lineno
        && آخرین_dot->انتها_ستون_جابه_جایی == module->ستون_جابه_جایی
    ) {
        أرجع 0;
    }

    صحيح شمار = Py_Pegen_seq_شمار_dots(dots);
    حرف *buf = PyMem_RawMalloc(شمار + 1);
    إذا (buf == عدم) {
        Py_Err_No_حافظه();
        أرجع -1;
    }
    memset(buf, '.', شمار);
    buf[شمار] = '\0';

    PyObject *پیام = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    إذا (پیام == عدم) {
        أرجع -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    صحيح res = _PyErr_EmitSyntaxWarning(پیام,
                                       info.filename,
                                       module->lineno,
                                       module->ستون_جابه_جایی + 1,
                                       module->انتها_lineno,
                                       module->انتها_ستون_جابه_جایی + 1,
                                       info.module);
    Py_DECREF(پیام);
    أرجع res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, عبارت_ty module_name,
                             asdl_alias_seq *نامها, عبارت_ty lazy_token, صحيح lineno,
                             صحيح ستون_جابه_جایی, صحيح انتها_lineno, صحيح انتها_ستون_جابه_جایی,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    صحيح level = Py_Pegen_seq_شمار_dots(dots);
    إذا (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        إذا (lazy_token) {
            RAISE_SYNTAX_ERROR_معلوم_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            أرجع عدم;
        }
        لكل (Py_ssize_t i = 0; i < asdl_seq_LEN(نامها); i++) {
            alias_ty alias = asdl_seq_GET(نامها, i);
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
        إذا (warn_relative_import_از_lazy(p, dots, module_name) < 0) {
            أرجع عدم;
        }
    }
    أرجع _PyAST_ImportFrom(module, نامها, level, lazy_token ? 1 : 0, lineno,
                             ستون_جابه_جایی, انتها_lineno, انتها_ستون_جابه_جایی, arena);
}

asdl_stmt_seq*
Py_Pegen_ثبات_stmts(Parser *p, asdl_stmt_seq* stmts) {
    إذا (!p->call_invalid_قواعد) {
        أرجع stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    إذا (len == 0) {
        أرجع stmts;
    }
    stmt_ty آخرین_stmt = asdl_seq_GET(stmts, len - 1);
    إذا (p->آخرین_stmt_location.lineno > آخرین_stmt->lineno) {
        أرجع stmts;
    }
    p->آخرین_stmt_location.lineno = آخرین_stmt->lineno;
    p->آخرین_stmt_location.ستون_جابه_جایی = آخرین_stmt->ستون_جابه_جایی;
    p->آخرین_stmt_location.انتها_lineno = آخرین_stmt->انتها_lineno;
    p->آخرین_stmt_location.انتها_ستون_جابه_جایی = آخرین_stmt->انتها_ستون_جابه_جایی;
    أرجع stmts;
}
