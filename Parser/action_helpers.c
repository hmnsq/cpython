#అన్తర్భావ <Python.h>
#అన్తర్భావ "pycore_pystate.h"         // _PyInterpreterState_GET()
#అన్తర్భావ "pycore_runtime.h"         // _PyRuntime
#అన్తర్భావ "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#అన్తర్భావ "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#అన్తర్భావ "pegen.h"
#అన్తర్భావ "string_parser.h"          // _PyPegen_decode_string()
#అన్తర్భావ "lexer/state.h"            // tok_state


శూన్య *
_PyPegen_dummy_name(Parser *p, ...)
{
    తిరిగి &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, శూన్య *a)
{
    ప్రతిజ్ఞా(a != ఖాళీ);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    ఒకవేళ (!seq) {
        తిరిగి ఖాళీ;
    }
    asdl_seq_సమితి_UNTYPED(seq, 0, a);
    తిరిగి seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_చొప్పించు_లోపల_front(Parser *p, శూన్య *a, asdl_seq *seq)
{
    ప్రతిజ్ఞా(a != ఖాళీ);
    ఒకవేళ (!seq) {
        తిరిగి _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }

    asdl_seq_సమితి_UNTYPED(new_seq, 0, a);
    కొరకు (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_సమితి_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    తిరిగి new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_గమ్యం_చివర(Parser *p, asdl_seq *seq, శూన్య *a)
{
    ప్రతిజ్ఞా(a != ఖాళీ);
    ఒకవేళ (!seq) {
        తిరిగి _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }

    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_సమితి_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_సమితి_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    తిరిగి new_seq;
}

స్థిర Py_ssize_t
get_flattened_seq_కొలత(asdl_seq *seqs)
{
    Py_ssize_t కొలత = 0;
    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        కొలత += asdl_seq_LEN(inner_seq);
    }
    తిరిగి కొలత;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_కొలత = get_flattened_seq_కొలత(seqs);
    ప్రతిజ్ఞా(flattened_seq_కొలత > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_కొలత, p->arena);
    ఒకవేళ (!flattened_seq) {
        తిరిగి ఖాళీ;
    }

    పూర్ణాంకం flattened_seq_idx = 0;
    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        కొరకు (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_సమితి_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    ప్రతిజ్ఞా(flattened_seq_idx == flattened_seq_కొలత);

    తిరిగి flattened_seq;
}

శూన్య *
Py_Pegen_seq_చివరి_అంశం(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    తిరిగి asdl_seq_GET_UNTYPED(seq, len - 1);
}

శూన్య *
Py_Pegen_seq_మొదటి_అంశం(asdl_seq *seq)
{
    తిరిగి asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
సమాసం_ty
Py_Pegen_join_పేర్లు_with_dot(Parser *p, సమాసం_ty మొదటి_name, సమాసం_ty రెండవ_name)
{
    ప్రతిజ్ఞా(మొదటి_name != ఖాళీ && రెండవ_name != ఖాళీ);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            మొదటి_name->v.Name.id, రెండవ_name->v.Name.id);
    ఒకవేళ (!uni) {
        తిరిగి ఖాళీ;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        తిరిగి ఖాళీ;
    }

    తిరిగి _PyAST_Name(uni, ఎక్కించు, EXTRA_సమాసం(మొదటి_name, రెండవ_name));
}

/* Counts the total number of dots in seq's tokens */
పూర్ణాంకం
Py_Pegen_seq_లెక్క_dots(asdl_seq *seq)
{
    పూర్ణాంకం సంఖ్య_యొక్క_dots = 0;
    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_సమాసం = asdl_seq_GET_UNTYPED(seq, i);
        ఎంపిక (current_సమాసం->type) {
            స్థితి ELLIPSIS:
                సంఖ్య_యొక్క_dots += 3;
                ఆపు;
            స్థితి DOT:
                సంఖ్య_యొక్క_dots += 1;
                ఆపు;
            అప్రమేయ:
                Py_UNREACHABLE();
        }
    }

    తిరిగి సంఖ్య_యొక్క_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_నక్షత్రం(Parser *p, పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                        పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    ఒకవేళ (!str) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_alias(str, ఖాళీ, lineno, నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_పేర్లు_గమ్యం_ids(Parser *p, asdl_సమాసం_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ప్రతిజ్ఞా(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty e = asdl_seq_GET(seq, i);
        asdl_seq_సమితి(new_seq, i, e->v.Name.id);
    }
    తిరిగి new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_సమాసం_Pair *
Py_Pegen_cmpop_సమాసం_pair(Parser *p, cmpop_ty cmpop, సమాసం_ty సమాసం)
{
    ప్రతిజ్ఞా(సమాసం != ఖాళీ);
    Cmpop_సమాసం_Pair *a = _PyArena_Malloc(p->arena, పరిమాణం(Cmpop_సమాసం_Pair));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->cmpop = cmpop;
    a->సమాసం = సమాసం;
    తిరిగి a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ప్రతిజ్ఞా(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_సమాసం_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->cmpop);
    }
    తిరిగి new_seq;
}

asdl_సమాసం_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ప్రతిజ్ఞా(len > 0);

    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_సమాసం_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->సమాసం);
    }
    తిరిగి new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
స్థిర asdl_సమాసం_seq *
సమితి_seq_context(Parser *p, asdl_సమాసం_seq *seq, సమాసం_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    ఒకవేళ (len == 0) {
        తిరిగి ఖాళీ;
    }

    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty e = asdl_seq_GET(seq, i);
        సమాసం_ty new_e = Py_Pegen_సమితి_సమాసం_context(p, e, ctx);
        ఒకవేళ (!new_e) {
            తిరిగి ఖాళీ;
        }
        asdl_seq_సమితి(new_seq, i, new_e);
    }
    తిరిగి new_seq;
}

స్థిర సమాసం_ty
సమితి_name_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    తిరిగి _PyAST_Name(e->v.Name.id, ctx, EXTRA_సమాసం(e, e));
}

స్థిర సమాసం_ty
సమితి_tuple_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    asdl_సమాసం_seq *seq = సమితి_seq_context(p, e->v.Tuple.elts, ctx);
    ఒకవేళ (!seq && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Tuple(seq, ctx, EXTRA_సమాసం(e, e));
}

స్థిర సమాసం_ty
సమితి_list_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    asdl_సమాసం_seq *seq = సమితి_seq_context(p, e->v.List.elts, ctx);
    ఒకవేళ (!seq && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_List(seq, ctx, EXTRA_సమాసం(e, e));
}

స్థిర సమాసం_ty
సమితి_subscript_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    తిరిగి _PyAST_Subscript(e->v.Subscript.విలువ, e->v.Subscript.ముక్క,
                            ctx, EXTRA_సమాసం(e, e));
}

స్థిర సమాసం_ty
సమితి_attribute_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    తిరిగి _PyAST_Attribute(e->v.Attribute.విలువ, e->v.Attribute.attr,
                            ctx, EXTRA_సమాసం(e, e));
}

స్థిర సమాసం_ty
సమితి_starred_context(Parser *p, సమాసం_ty e, సమాసం_context_ty ctx)
{
    సమాసం_ty inner = Py_Pegen_సమితి_సమాసం_context(p, e->v.Starred.విలువ, ctx);
    ఒకవేళ (!inner) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Starred(inner, ctx, EXTRA_సమాసం(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
సమాసం_ty
Py_Pegen_సమితి_సమాసం_context(Parser *p, సమాసం_ty సమాసం, సమాసం_context_ty ctx)
{
    ప్రతిజ్ఞా(సమాసం != ఖాళీ);

    సమాసం_ty new = ఖాళీ;
    ఎంపిక (సమాసం->రకం) {
        స్థితి Name_రకం:
            new = సమితి_name_context(p, సమాసం, ctx);
            ఆపు;
        స్థితి Tuple_రకం:
            new = సమితి_tuple_context(p, సమాసం, ctx);
            ఆపు;
        స్థితి List_రకం:
            new = సమితి_list_context(p, సమాసం, ctx);
            ఆపు;
        స్థితి Subscript_రకం:
            new = సమితి_subscript_context(p, సమాసం, ctx);
            ఆపు;
        స్థితి Attribute_రకం:
            new = సమితి_attribute_context(p, సమాసం, ctx);
            ఆపు;
        స్థితి Starred_రకం:
            new = సమితి_starred_context(p, సమాసం, ctx);
            ఆపు;
        అప్రమేయ:
            new = సమాసం;
    }
    తిరిగి new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
సమాసం_ty
Py_Pegen_make_లక్ష్యం(Parser *p, సమాసం_ty సమాసం, TARGETS_TYPE targets_type)
{
    ప్రతిజ్ఞా(సమాసం != ఖాళీ);
    ప్రతిజ్ఞా(targets_type != FOR_TARGETS);
    ఒకవేళ (Py_Pegen_get_invalid_లక్ష్యం(సమాసం, targets_type) != ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    తిరిగి Py_Pegen_సమితి_సమాసం_context(
        p, సమాసం, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
కీలకం_విలువ_Pair *
Py_Pegen_కీలకం_విలువ_pair(Parser *p, సమాసం_ty కీలకం, సమాసం_ty విలువ)
{
    కీలకం_విలువ_Pair *a = _PyArena_Malloc(p->arena, పరిమాణం(కీలకం_విలువ_Pair));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->కీలకం = కీలకం;
    a->విలువ = విలువ;
    తిరిగి a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_సమాసం_seq *
Py_Pegen_get_కీలకాలు(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        కీలకం_విలువ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->కీలకం);
    }
    తిరిగి new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_సమాసం_seq *
Py_Pegen_get_విలువలు(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        కీలకం_విలువ_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->విలువ);
    }
    తిరిగి new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
కీలకం_నమూనా_Pair *
Py_Pegen_కీలకం_నమూనా_pair(Parser *p, సమాసం_ty కీలకం, నమూనా_ty నమూనా)
{
    కీలకం_నమూనా_Pair *a = _PyArena_Malloc(p->arena, పరిమాణం(కీలకం_నమూనా_Pair));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->కీలకం = కీలకం;
    a->నమూనా = నమూనా;
    తిరిగి a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_సమాసం_seq *
Py_Pegen_get_నమూనా_కీలకాలు(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        కీలకం_నమూనా_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->కీలకం);
    }
    తిరిగి new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_నమూనా_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_నమూనా_seq *new_seq = Py_asdl_నమూనా_seq_new(len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        కీలకం_నమూనా_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_సమితి(new_seq, i, pair->నమూనా);
    }
    తిరిగి new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, సమాసం_ty విలువ, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, పరిమాణం(NameDefaultPair));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->arg = Py_Pegen_కూడు_type_comment_గమ్యం_arg(p, arg, tc);
    ఒకవేళ (!a->arg) {
        తిరిగి ఖాళీ;
    }
    a->విలువ = విలువ;
    తిరిగి a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_పేర్లు, asdl_seq *పేర్లు_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, పరిమాణం(SlashWithDefault));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->plain_పేర్లు = plain_పేర్లు;
    a->పేర్లు_with_defaults = పేర్లు_with_defaults;
    తిరిగి a;
}

/* Constructs a StarEtc */
నక్షత్రం_Etc *
Py_Pegen_నక్షత్రం_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    నక్షత్రం_Etc *a = _PyArena_Malloc(p->arena, పరిమాణం(నక్షత్రం_Etc));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    తిరిగి a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t మొదటి_len = asdl_seq_LEN(a);
    Py_ssize_t రెండవ_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(మొదటి_len + రెండవ_len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }

    పూర్ణాంకం k = 0;
    కొరకు (Py_ssize_t i = 0; i < మొదటి_len; i++) {
        asdl_seq_సమితి_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    కొరకు (Py_ssize_t i = 0; i < రెండవ_len; i++) {
        asdl_seq_సమితి_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    తిరిగి new_seq;
}

స్థిర asdl_arg_seq*
get_పేర్లు(Parser *p, asdl_seq *పేర్లు_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(పేర్లు_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    ఒకవేళ (!seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(పేర్లు_with_defaults, i);
        asdl_seq_సమితి(seq, i, pair->arg);
    }
    తిరిగి seq;
}

స్థిర asdl_సమాసం_seq *
_get_defaults(Parser *p, asdl_seq *పేర్లు_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(పేర్లు_with_defaults);
    asdl_సమాసం_seq *seq = Py_asdl_సమాసం_seq_new(len, p->arena);
    ఒకవేళ (!seq) {
        తిరిగి ఖాళీ;
    }
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(పేర్లు_with_defaults, i);
        asdl_seq_సమితి(seq, i, pair->విలువ);
    }
    తిరిగి seq;
}

స్థిర పూర్ణాంకం
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    ఒకవేళ (slash_without_default != ఖాళీ) {
        *posonlyargs = slash_without_default;
    }
    లేకపోతే ఒకవేళ (slash_with_default != ఖాళీ) {
        asdl_arg_seq *slash_with_default_పేర్లు =
                get_పేర్లు(p, slash_with_default->పేర్లు_with_defaults);
        ఒకవేళ (!slash_with_default_పేర్లు) {
            తిరిగి -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_పేర్లు,
                (asdl_seq*)slash_with_default_పేర్లు);
    }
    లేకపోతే {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    తిరిగి *posonlyargs == ఖాళీ ? -1 : 0;
}

స్థిర పూర్ణాంకం
_make_posargs(Parser *p,
              asdl_arg_seq *plain_పేర్లు,
              asdl_seq *పేర్లు_with_default,
              asdl_arg_seq **posargs) {

    ఒకవేళ (పేర్లు_with_default != ఖాళీ) {
        ఒకవేళ (plain_పేర్లు != ఖాళీ) {
            asdl_arg_seq *పేర్లు_with_default_పేర్లు = get_పేర్లు(p, పేర్లు_with_default);
            ఒకవేళ (!పేర్లు_with_default_పేర్లు) {
                తిరిగి -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_పేర్లు, (asdl_seq*)పేర్లు_with_default_పేర్లు);
        }
        లేకపోతే {
            *posargs = get_పేర్లు(p, పేర్లు_with_default);
        }
    }
    లేకపోతే {
        ఒకవేళ (plain_పేర్లు != ఖాళీ) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            ప్రతిజ్ఞా(0);
            *posargs = plain_పేర్లు;
        }
        లేకపోతే {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    తిరిగి *posargs == ఖాళీ ? -1 : 0;
}

స్థిర పూర్ణాంకం
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *పేర్లు_with_default,
                  asdl_సమాసం_seq **posdefaults) {
    ఒకవేళ (slash_with_default != ఖాళీ && పేర్లు_with_default != ఖాళీ) {
        asdl_సమాసం_seq *slash_with_default_విలువలు =
                _get_defaults(p, slash_with_default->పేర్లు_with_defaults);
        ఒకవేళ (!slash_with_default_విలువలు) {
            తిరిగి -1;
        }
        asdl_సమాసం_seq *పేర్లు_with_default_విలువలు = _get_defaults(p, పేర్లు_with_default);
        ఒకవేళ (!పేర్లు_with_default_విలువలు) {
            తిరిగి -1;
        }
        *posdefaults = (asdl_సమాసం_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_విలువలు,
                (asdl_seq*)పేర్లు_with_default_విలువలు);
    }
    లేకపోతే ఒకవేళ (slash_with_default == ఖాళీ && పేర్లు_with_default != ఖాళీ) {
        *posdefaults = _get_defaults(p, పేర్లు_with_default);
    }
    లేకపోతే ఒకవేళ (slash_with_default != ఖాళీ && పేర్లు_with_default == ఖాళీ) {
        *posdefaults = _get_defaults(p, slash_with_default->పేర్లు_with_defaults);
    }
    లేకపోతే {
        *posdefaults = Py_asdl_సమాసం_seq_new(0, p->arena);
    }
    తిరిగి *posdefaults == ఖాళీ ? -1 : 0;
}

స్థిర పూర్ణాంకం
_make_kwargs(Parser *p, నక్షత్రం_Etc *నక్షత్రం_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_సమాసం_seq **kwdefaults) {
    ఒకవేళ (నక్షత్రం_etc != ఖాళీ && నక్షత్రం_etc->kwonlyargs != ఖాళీ) {
        *kwonlyargs = get_పేర్లు(p, నక్షత్రం_etc->kwonlyargs);
    }
    లేకపోతే {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    ఒకవేళ (*kwonlyargs == ఖాళీ) {
        తిరిగి -1;
    }

    ఒకవేళ (నక్షత్రం_etc != ఖాళీ && నక్షత్రం_etc->kwonlyargs != ఖాళీ) {
        *kwdefaults = _get_defaults(p, నక్షత్రం_etc->kwonlyargs);
    }
    లేకపోతే {
        *kwdefaults = Py_asdl_సమాసం_seq_new(0, p->arena);
    }

    ఒకవేళ (*kwdefaults == ఖాళీ) {
        తిరిగి -1;
    }

    తిరిగి 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_పేర్లు,
                        asdl_seq *పేర్లు_with_default, నక్షత్రం_Etc *నక్షత్రం_etc)
{
    asdl_arg_seq *posonlyargs;
    ఒకవేళ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        తిరిగి ఖాళీ;
    }

    asdl_arg_seq *posargs;
    ఒకవేళ (_make_posargs(p, plain_పేర్లు, పేర్లు_with_default, &posargs) == -1) {
        తిరిగి ఖాళీ;
    }

    asdl_సమాసం_seq *posdefaults;
    ఒకవేళ (_make_posdefaults(p,slash_with_default, పేర్లు_with_default, &posdefaults) == -1) {
        తిరిగి ఖాళీ;
    }

    arg_ty vararg = ఖాళీ;
    ఒకవేళ (నక్షత్రం_etc != ఖాళీ && నక్షత్రం_etc->vararg != ఖాళీ) {
        vararg = నక్షత్రం_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_సమాసం_seq *kwdefaults;
    ఒకవేళ (_make_kwargs(p, నక్షత్రం_etc, &kwonlyargs, &kwdefaults) == -1) {
        తిరిగి ఖాళీ;
    }

    arg_ty kwarg = ఖాళీ;
    ఒకవేళ (నక్షత్రం_etc != ఖాళీ && నక్షత్రం_etc->kwarg != ఖాళీ) {
        kwarg = నక్షత్రం_etc->kwarg;
    }

    తిరిగి _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_ఖాళీలు_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ఒకవేళ (!posonlyargs) {
        తిరిగి ఖాళీ;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    ఒకవేళ (!posargs) {
        తిరిగి ఖాళీ;
    }
    asdl_సమాసం_seq *posdefaults = Py_asdl_సమాసం_seq_new(0, p->arena);
    ఒకవేళ (!posdefaults) {
        తిరిగి ఖాళీ;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    ఒకవేళ (!kwonlyargs) {
        తిరిగి ఖాళీ;
    }
    asdl_సమాసం_seq *kwdefaults = Py_asdl_సమాసం_seq_new(0, p->arena);
    ఒకవేళ (!kwdefaults) {
        తిరిగి ఖాళీ;
    }

    తిరిగి _PyAST_arguments(posonlyargs, posargs, ఖాళీ, kwonlyargs,
                            kwdefaults, ఖాళీ, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty రకం)
{
    AugOperator *a = _PyArena_Malloc(p->arena, పరిమాణం(AugOperator));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->రకం = రకం;
    తిరిగి a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_సమాసం_seq *decorators, stmt_ty function_def)
{
    ప్రతిజ్ఞా(function_def != ఖాళీ);
    ఒకవేళ (function_def->రకం == Async_Function_Def_రకం) {
        తిరిగి _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.వాదనలు,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->నిలువు_స్థానభ్రంశం,
            function_def->చివర_lineno, function_def->చివర_నిలువు_స్థానభ్రంశం, p->arena);
    }

    తిరిగి _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.వాదనలు,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->నిలువు_స్థానభ్రంశం,
        function_def->చివర_lineno, function_def->చివర_నిలువు_స్థానభ్రంశం, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_సమాసం_seq *decorators, stmt_ty class_def)
{
    ప్రతిజ్ఞా(class_def != ఖాళీ);
    తిరిగి _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->నిలువు_స్థానభ్రంశం, class_def->చివర_lineno,
        class_def->చివర_నిలువు_స్థానభ్రంశం, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, శూన్య *element, పూర్ణాంకం is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, పరిమాణం(KeywordOrStarred));
    ఒకవేళ (!a) {
        తిరిగి ఖాళీ;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    తిరిగి a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
స్థిర పూర్ణాంకం
seq_సంఖ్య_యొక్క_starred_exprs(asdl_seq *seq)
{
    పూర్ణాంకం n = 0;
    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        ఒకవేళ (!k->is_keyword) {
            n++;
        }
    }
    తిరిగి n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_సమాసం_seq *
Py_Pegen_seq_వెలికితీయి_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    పూర్ణాంకం new_len = seq_సంఖ్య_యొక్క_starred_exprs(kwargs);
    ఒకవేళ (new_len == 0) {
        తిరిగి ఖాళీ;
    }
    asdl_సమాసం_seq *new_seq = Py_asdl_సమాసం_seq_new(new_len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }

    పూర్ణాంకం idx = 0;
    కొరకు (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ఒకవేళ (!k->is_keyword) {
            asdl_seq_సమితి(new_seq, idx++, k->element);
        }
    }
    తిరిగి new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_సంఖ్య_యొక్క_starred_exprs(kwargs);
    ఒకవేళ (new_len == 0) {
        తిరిగి ఖాళీ;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    ఒకవేళ (!new_seq) {
        తిరిగి ఖాళీ;
    }

    పూర్ణాంకం idx = 0;
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        ఒకవేళ (k->is_keyword) {
            asdl_seq_సమితి(new_seq, idx++, k->element);
        }
    }
    తిరిగి new_seq;
}

సమాసం_ty
_PyPegen_ensure_imaginary(Parser *p, సమాసం_ty ఘాతాంకం)
{
    ఒకవేళ (ఘాతాంకం->రకం != Constant_రకం || !PyComplex_CheckExact(ఘాతాంకం->v.Constant.విలువ)) {
        RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(ఘాతాంకం, "imaginary number required in complex literal");
        తిరిగి ఖాళీ;
    }
    తిరిగి ఘాతాంకం;
}

సమాసం_ty
_PyPegen_ensure_real(Parser *p, సమాసం_ty ఘాతాంకం)
{
    ఒకవేళ (ఘాతాంకం->రకం != Constant_రకం || PyComplex_CheckExact(ఘాతాంకం->v.Constant.విలువ)) {
        RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(ఘాతాంకం, "real number required in complex literal");
        తిరిగి ఖాళీ;
    }
    తిరిగి ఘాతాంకం;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = ఖాళీ;
    Py_ssize_t అంకె = p->type_ignore_comments.అంకె_వస్తువులు;
    ఒకవేళ (అంకె > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(అంకె, p->arena);
        ఒకవేళ (type_ignores == ఖాళీ) {
            తిరిగి ఖాళీ;
        }
        కొరకు (Py_ssize_t i = 0; i < అంకె; i++) {
            PyObject *ట్యాగ్ = _PyPegen_new_type_comment(p, p->type_ignore_comments.వస్తువులు[i].comment);
            ఒకవేళ (ట్యాగ్ == ఖాళీ) {
                తిరిగి ఖాళీ;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.వస్తువులు[i].lineno,
                                                  ట్యాగ్, p->arena);
            ఒకవేళ (ti == ఖాళీ) {
                తిరిగి ఖాళీ;
            }
            asdl_seq_సమితి(type_ignores, i, ti);
        }
    }
    తిరిగి _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, స్థిరరాశి అక్షరం *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, అక్షరశ్రేణి_పొడవు(s), ఖాళీ);
    ఒకవేళ (res == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        తిరిగి ఖాళీ;
    }
    తిరిగి res;
}

arg_ty
Py_Pegen_కూడు_type_comment_గమ్యం_arg(Parser *p, arg_ty a, Token *tc)
{
    ఒకవేళ (tc == ఖాళీ) {
        తిరిగి a;
    }
    స్థిరరాశి అక్షరం *బైట్లు = Py_బైట్లు_As_String(tc->బైట్లు);
    ఒకవేళ (బైట్లు == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, బైట్లు);
    ఒకవేళ (tco == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->నిలువు_స్థానభ్రంశం, a->చివర_lineno, a->చివర_నిలువు_స్థానభ్రంశం,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
పూర్ణాంకం
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    ప్రతిజ్ఞా(t->బైట్లు != ఖాళీ);
    ప్రతిజ్ఞా(t->type == NOTEQUAL);

    స్థిరరాశి అక్షరం* tok_str = Py_బైట్లు_AS_STRING(t->బైట్లు);
    ఒకవేళ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        తిరిగి -1;
    }
    ఒకవేళ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        తిరిగి strcmp(tok_str, "!=");
    }
    తిరిగి 0;
}

పూర్ణాంకం
_PyPegen_check_legacy_stmt(Parser *p, సమాసం_ty name) {
    ఒకవేళ (name->రకం != Name_రకం) {
        తిరిగి 0;
    }
    స్థిరరాశి అక్షరం* candidates[2] = {"print", "exec"};
    కొరకు (పూర్ణాంకం i=0; i<2; i++) {
        ఒకవేళ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            తిరిగి 1;
        }
    }
    తిరిగి 0;
}

శూన్య *
_PyPegen_raise_error_for_missing_comma(Parser *p, సమాసం_ty a, సమాసం_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    ఒకవేళ (_PyPegen_check_legacy_stmt(p, a)) {
        తిరిగి ఖాళీ;
    }
    // Only raise inside parentheses/brackets (level > 0)
    ఒకవేళ (p->టోకెన్లు[p->mark - 1]->level == 0) {
        తిరిగి ఖాళీ;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    ఒకవేళ (a->చివర_lineno > a->lineno) {
        తిరిగి RAISE_ERROR_తెలిసిన_LOCATION(
            p, PyExc_SyntaxError, a->చివర_lineno, a->నిలువు_స్థానభ్రంశం,
            a->చివర_lineno, a->చివర_నిలువు_స్థానభ్రంశం,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    తిరిగి RAISE_ERROR_తెలిసిన_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->నిలువు_స్థానభ్రంశం,
        b->చివర_lineno, b->చివర_నిలువు_స్థానభ్రంశం,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

స్థిర ఫలితం_Token_With_Metadata *
ఫలితం_token_with_metadata(Parser *p, శూన్య *ఫలితం, PyObject *metadata)
{
    ఫలితం_Token_With_Metadata *res = _PyArena_Malloc(p->arena, పరిమాణం(ఫలితం_Token_With_Metadata));
    ఒకవేళ (res == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    res->metadata = metadata;
    res->ఫలితం = ఫలితం;
    తిరిగి res;
}

స్థిర అక్షరం
formatted_string_ఉపసర్గ(స్థిరరాశి Parser *p)
{
    పూర్ణాంకం nested = 0;
    కొరకు (పూర్ణాంకం i = p->mark - 1; i >= 0; i--) {
        పూర్ణాంకం type = p->టోకెన్లు[i]->type;
        ఒకవేళ (type == FSTRING_చివర || type == TSTRING_చివర) {
            nested++;
        }
        లేకపోతే ఒకవేళ (type == FSTRING_ప్రారంభం || type == TSTRING_ప్రారంభం) {
            ఒకవేళ (nested == 0) {
                తిరిగి type == TSTRING_ప్రారంభం ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ఫలితం_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, సమాసం_ty conv)
{
    ఒకవేళ (conv_token->lineno != conv->lineno || conv_token->చివర_నిలువు_స్థానభ్రంశం != conv->నిలువు_స్థానభ్రంశం) {
        తిరిగి RAISE_SYNTAX_ERROR_తెలిసిన_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_ఉపసర్గ(p)
        );
    }

    Py_UCS4 మొదటి = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    ఒకవేళ (Py_Unicode_GET_నిడివి(conv->v.Name.id) > 1 ||
            !(మొదటి == 's' || మొదటి == 'r' || మొదటి == 'a')) {
        RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_ఉపసర్గ(p),
                                            conv->v.Name.id);
        తిరిగి ఖాళీ;
    }

    తిరిగి ఫలితం_token_with_metadata(p, conv, conv_token->metadata);
}

ఫలితం_Token_With_Metadata *
Py_Pegen_అమరిక_నిండిన_format_spec(Parser *p, Token *colon, asdl_సమాసం_seq *spec, పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం,
                                పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena)
{
    ఒకవేళ (!spec) {
        తిరిగి ఖాళీ;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_వస్తువులు = asdl_seq_LEN(spec);
    Py_ssize_t non_ఖాళీలు_లెక్క = 0;
    కొరకు (Py_ssize_t i = 0; i < n_వస్తువులు; i++) {
        సమాసం_ty అంశం = asdl_seq_GET(spec, i);
        non_ఖాళీలు_లెక్క += !(అంశం->రకం == Constant_రకం &&
                             PyUnicode_CheckExact(అంశం->v.Constant.విలువ) &&
                             Py_Unicode_GET_నిడివి(అంశం->v.Constant.విలువ) == 0);
    }
    ఒకవేళ (non_ఖాళీలు_లెక్క != n_వస్తువులు) {
        asdl_సమాసం_seq *resized_spec =
            Py_asdl_సమాసం_seq_new(non_ఖాళీలు_లెక్క, p->arena);
        ఒకవేళ (resized_spec == ఖాళీ) {
            తిరిగి ఖాళీ;
        }
        Py_ssize_t j = 0;
        కొరకు (Py_ssize_t i = 0; i < n_వస్తువులు; i++) {
            సమాసం_ty అంశం = asdl_seq_GET(spec, i);
            ఒకవేళ (అంశం->రకం == Constant_రకం &&
                PyUnicode_CheckExact(అంశం->v.Constant.విలువ) &&
                Py_Unicode_GET_నిడివి(అంశం->v.Constant.విలువ) == 0) {
                కొనసాగు;
            }
            asdl_seq_సమితి(resized_spec, j++, అంశం);
        }
        ప్రతిజ్ఞా(j == non_ఖాళీలు_లెక్క);
        spec = resized_spec;
    }
    సమాసం_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    ఒకవేళ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->రకం == Constant_రకం)) {
        res = _PyAST_JoinedStr(spec, lineno, నిలువు_స్థానభ్రంశం, చివర_lineno,
                                    చివర_నిలువు_స్థానభ్రంశం, p->arena);
    } లేకపోతే {
        res = Py_Pegen_concatenate_అక్షరశ్రేణులు(p, spec,
                             lineno, నిలువు_స్థానభ్రంశం, చివర_lineno,
                             చివర_నిలువు_స్థానభ్రంశం, arena);
    }
    ఒకవేళ (!res) {
        తిరిగి ఖాళీ;
    }
    తిరిగి ఫలితం_token_with_metadata(p, res, colon->metadata);
}

స్థిరరాశి అక్షరం *
Py_Pegen_get_సమాసం_name(సమాసం_ty e)
{
    ప్రతిజ్ఞా(e != ఖాళీ);
    ఎంపిక (e->రకం) {
        స్థితి Attribute_రకం:
            తిరిగి "attribute";
        స్థితి Subscript_రకం:
            తిరిగి "subscript";
        స్థితి Starred_రకం:
            తిరిగి "starred";
        స్థితి Name_రకం:
            తిరిగి "name";
        స్థితి List_రకం:
            తిరిగి "list";
        స్థితి Tuple_రకం:
            తిరిగి "tuple";
        స్థితి Lambda_రకం:
            తిరిగి "lambda";
        స్థితి Call_రకం:
            తిరిగి "function call";
        స్థితి Bool_Op_రకం:
        స్థితి Bin_Op_రకం:
        స్థితి Unary_Op_రకం:
            తిరిగి "expression";
        స్థితి Generator_Exp_రకం:
            తిరిగి "generator expression";
        స్థితి Yield_రకం:
        స్థితి Yield_From_రకం:
            తిరిగి "yield expression";
        స్థితి Await_రకం:
            తిరిగి "await expression";
        స్థితి List_Comp_రకం:
            తిరిగి "list comprehension";
        స్థితి సమితి_Comp_రకం:
            తిరిగి "set comprehension";
        స్థితి Dict_Comp_రకం:
            తిరిగి "dict comprehension";
        స్థితి Dict_రకం:
            తిరిగి "dict literal";
        స్థితి సమితి_రకం:
            తిరిగి "set display";
        స్థితి Joined_Str_రకం:
        స్థితి Formatted_విలువ_రకం:
            తిరిగి "f-string expression";
        స్థితి Template_Str_రకం:
        స్థితి Interpolation_రకం:
            తిరిగి "t-string expression";
        స్థితి Constant_రకం: {
            PyObject *విలువ = e->v.Constant.విలువ;
            ఒకవేళ (విలువ == Py_None) {
                తిరిగి "None";
            }
            ఒకవేళ (విలువ == Py_False) {
                తిరిగి "False";
            }
            ఒకవేళ (విలువ == Py_True) {
                తిరిగి "True";
            }
            ఒకవేళ (విలువ == Py_Ellipsis) {
                తిరిగి "ellipsis";
            }
            తిరిగి "literal";
        }
        స్థితి Compare_రకం:
            తిరిగి "comparison";
        స్థితి If_Exp_రకం:
            తిరిగి "conditional expression";
        స్థితి Named_సమాసం_రకం:
            తిరిగి "named expression";
        అప్రమేయ:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->రకం, e->lineno);
            తిరిగి ఖాళీ;
    }
}

సమాసం_ty
Py_Pegen_get_చివరి_comprehension_అంశం(comprehension_ty comprehension) {
    ఒకవేళ (comprehension->ifs == ఖాళీ || asdl_seq_LEN(comprehension->ifs) == 0) {
        తిరిగి comprehension->iter;
    }
    తిరిగి Py_Pegen_చివరి_అంశం(comprehension->ifs, సమాసం_ty);
}

సమాసం_ty _PyPegen_collect_call_seqs(Parser *p, asdl_సమాసం_seq *a, asdl_seq *b,
                     పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                     పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena) {
    Py_ssize_t వాదనలు_len = asdl_seq_LEN(a);
    Py_ssize_t మొత్తం_len = వాదనలు_len;

    ఒకవేళ (b == ఖాళీ) {
        తిరిగి _PyAST_Call(_PyPegen_dummy_name(p), a, ఖాళీ, lineno, నిలువు_స్థానభ్రంశం,
                        చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);

    }

    asdl_సమాసం_seq *starreds = Py_Pegen_seq_వెలికితీయి_starred_exprs(p, b);
    ఒకవేళ (!starreds && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    ఒకవేళ (!keywords && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (starreds) {
        మొత్తం_len += asdl_seq_LEN(starreds);
    }

    asdl_సమాసం_seq *వాదనలు = Py_asdl_సమాసం_seq_new(మొత్తం_len, arena);
    ఒకవేళ (వాదనలు == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    Py_ssize_t i = 0;
    కొరకు (i = 0; i < వాదనలు_len; i++) {
        asdl_seq_సమితి(వాదనలు, i, asdl_seq_GET(a, i));
    }
    కొరకు (; i < మొత్తం_len; i++) {
        asdl_seq_సమితి(వాదనలు, i, asdl_seq_GET(starreds, i - వాదనలు_len));
    }

    తిరిగి _PyAST_Call(_PyPegen_dummy_name(p), వాదనలు, keywords, lineno,
                       నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
}

// AST Error reporting helpers

సమాసం_ty
Py_Pegen_get_invalid_లక్ష్యం(సమాసం_ty e, TARGETS_TYPE targets_type)
{
    ఒకవేళ (e == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    ఒకవేళ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        ఒకవేళ (targets_type == SINGLE_TARGETS && e->రకం == Name_రకం) {
            తిరిగి ఖాళీ;
        }
        తిరిగి e->రకం == Attribute_రకం || e->రకం == Subscript_రకం ? ఖాళీ : e;
    }

#నిర్వచించు సందర్శించు_CONTAINER(CONTAINER, TYPE) చేయి { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        కొరకు (Py_ssize_t i = 0; i < len; i++) {\
            సమాసం_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            సమాసం_ty బిడ్డ = Py_Pegen_get_invalid_లక్ష్యం(other, targets_type);\
            ఒకవేళ (బిడ్డ != ఖాళీ) {\
                తిరిగి బిడ్డ;\
            }\
        }\
    } అయితే (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    ఎంపిక (e->రకం) {
        స్థితి List_రకం:
            సందర్శించు_CONTAINER(e, List);
            తిరిగి ఖాళీ;
        స్థితి Tuple_రకం:
            సందర్శించు_CONTAINER(e, Tuple);
            తిరిగి ఖాళీ;
        స్థితి Starred_రకం:
            ఒకవేళ (targets_type == DEL_TARGETS) {
                తిరిగి e;
            }
            తిరిగి Py_Pegen_get_invalid_లక్ష్యం(e->v.Starred.విలువ, targets_type);
        స్థితి Compare_రకం:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            ఒకవేళ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                ఒకవేళ (cmpop == లోపల) {
                    తిరిగి Py_Pegen_get_invalid_లక్ష్యం(e->v.Compare.ఎడమ, targets_type);
                }
                తిరిగి ఖాళీ;
            }
            తిరిగి e;
        స్థితి Name_రకం:
        స్థితి Subscript_రకం:
        స్థితి Attribute_రకం:
            తిరిగి ఖాళీ;
        అప్రమేయ:
            తిరిగి e;
    }
}

శూన్య *_PyPegen_arguments_parsing_error(Parser *p, సమాసం_ty e) {
    పూర్ణాంకం kwarg_unpacking = 0;
    కొరకు (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        ఒకవేళ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    స్థిరరాశి అక్షరం *సందేశం = ఖాళీ;
    ఒకవేళ (kwarg_unpacking) {
        సందేశం = "positional argument follows keyword argument unpacking";
    } లేకపోతే {
        సందేశం = "positional argument follows keyword argument";
    }

    తిరిగి RAISE_SYNTAX_ERROR(సందేశం);
}

శూన్య *
Py_Pegen_nonparen_genexp_లోపల_call(Parser *p, సమాసం_ty వాదనలు, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(వాదనలు->v.Call.వాదనలు);
    ఒకవేళ (len <= 1) {
        తిరిగి ఖాళీ;
    }

    comprehension_ty చివరి_comprehension = Py_Pegen_చివరి_అంశం(comprehensions, comprehension_ty);

    తిరిగి RAISE_SYNTAX_ERROR_తెలిసిన_RANGE(
        (సమాసం_ty) asdl_seq_GET(వాదనలు->v.Call.వాదనలు, len - 1),
        Py_Pegen_get_చివరి_comprehension_అంశం(చివరి_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

స్థిర సమాసం_ty
_PyPegen_decode_fstring_part(Parser* p, పూర్ణాంకం is_raw, సమాసం_ty constant, Token* token) {
    ప్రతిజ్ఞా(PyUnicode_CheckExact(constant->v.Constant.విలువ));

    స్థిరరాశి అక్షరం* bstr = PyUnicode_AsUTF8(constant->v.Constant.విలువ);
    ఒకవేళ (bstr == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    కొలత_t len;
    ఒకవేళ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } లేకపోతే {
        len = అక్షరశ్రేణి_పొడవు(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == ఖాళీ;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    ఒకవేళ (str == ఖాళీ) {
        _Pypegen_raise_decode_error(p);
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Constant(str, ఖాళీ, constant->lineno, constant->నిలువు_స్థానభ్రంశం,
                           constant->చివర_lineno, constant->చివర_నిలువు_స్థానభ్రంశం,
                           p->arena);
}

స్థిర asdl_సమాసం_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_సమాసం_seq *raw_expressions,
                   Token *b, పూర్ణాంకం is_tstring)
{
    Py_ssize_t n_వస్తువులు = asdl_seq_LEN(raw_expressions);
    Py_ssize_t మొత్తం_వస్తువులు = n_వస్తువులు;
    కొరకు (Py_ssize_t i = 0; i < n_వస్తువులు; i++) {
        సమాసం_ty అంశం = asdl_seq_GET(raw_expressions, i);
        ఒకవేళ (అంశం->రకం == Joined_Str_రకం) {
            మొత్తం_వస్తువులు += asdl_seq_LEN(అంశం->v.JoinedStr.విలువలు) - 1;
        }
    }

    స్థిరరాశి అక్షరం* quote_str = Py_బైట్లు_As_String(a->బైట్లు);
    ఒకవేళ (quote_str == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    పూర్ణాంకం is_raw = strpbrk(quote_str, "rR") != ఖాళీ;

    asdl_సమాసం_seq *seq = Py_asdl_సమాసం_seq_new(మొత్తం_వస్తువులు, p->arena);
    ఒకవేళ (seq == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    Py_ssize_t సూచీ = 0;
    కొరకు (Py_ssize_t i = 0; i < n_వస్తువులు; i++) {
        సమాసం_ty అంశం = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        ఒకవేళ (అంశం->రకం == Joined_Str_రకం) {
            asdl_సమాసం_seq *విలువలు = అంశం->v.JoinedStr.విలువలు;
            ఒకవేళ (asdl_seq_LEN(విలువలు) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             అంశం->lineno);
                తిరిగి ఖాళీ;
            }

            సమాసం_ty మొదటి = asdl_seq_GET(విలువలు, 0);
            ప్రతిజ్ఞా(మొదటి->రకం == Constant_రకం);
            asdl_seq_సమితి(seq, సూచీ++, మొదటి);

            సమాసం_ty రెండవ = asdl_seq_GET(విలువలు, 1);
            ప్రతిజ్ఞా((is_tstring &&
                    రెండవ->రకం == Interpolation_రకం) ||
                   రెండవ->రకం == Formatted_విలువ_రకం);
            asdl_seq_సమితి(seq, సూచీ++, రెండవ);

            కొనసాగు;
        }

        ఒకవేళ (అంశం->రకం == Constant_రకం) {
            అంశం = _PyPegen_decode_fstring_part(p, is_raw, అంశం, b);
            ఒకవేళ (అంశం == ఖాళీ) {
                తిరిగి ఖాళీ;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            ఒకవేళ (PyUnicode_CheckExact(అంశం->v.Constant.విలువ)
                && Py_Unicode_GET_నిడివి(అంశం->v.Constant.విలువ) == 0) {
                కొనసాగు;
            }
        }
        asdl_seq_సమితి(seq, సూచీ++, అంశం);
    }

    asdl_సమాసం_seq *resized_exprs;
    ఒకవేళ (సూచీ != మొత్తం_వస్తువులు) {
        resized_exprs = Py_asdl_సమాసం_seq_new(సూచీ, p->arena);
        ఒకవేళ (resized_exprs == ఖాళీ) {
            తిరిగి ఖాళీ;
        }
        కొరకు (Py_ssize_t i = 0; i < సూచీ; i++) {
            asdl_seq_సమితి(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    లేకపోతే {
        resized_exprs = seq;
    }
    తిరిగి resized_exprs;
}

సమాసం_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_సమాసం_seq *raw_expressions, Token *b) {

    asdl_సమాసం_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    ఒకవేళ (resized_exprs == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_TemplateStr(resized_exprs, a->lineno, a->నిలువు_స్థానభ్రంశం,
                              b->చివర_lineno, b->చివర_నిలువు_స్థానభ్రంశం,
                              p->arena);
}

సమాసం_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_సమాసం_seq* raw_expressions, Token*b) {

    asdl_సమాసం_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    ఒకవేళ (resized_exprs == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_JoinedStr(resized_exprs, a->lineno, a->నిలువు_స్థానభ్రంశం,
                            b->చివర_lineno, b->చివర_నిలువు_స్థానభ్రంశం,
                            p->arena);
}

సమాసం_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    ఒకవేళ (tok->బైట్లు == ఖాళీ) {
        తిరిగి Py_Pegen_raise_incomplete_ఆదానం(p);
    }
    Py_ssize_t bsize = Py_బైట్లు_GET_కొలత(tok->బైట్లు);
    స్థిరరాశి అక్షరం *bstr = Py_బైట్లు_AS_STRING(tok->బైట్లు);

    పూర్ణాంకం is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    ఒకవేళ (str == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Constant(str, ఖాళీ, tok->lineno, tok->నిలువు_స్థానభ్రంశం,
                           tok->చివర_lineno, tok->చివర_నిలువు_స్థానభ్రంశం,
                           p->arena);
}

సమాసం_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    ఒకవేళ (tok->బైట్లు == ఖాళీ) {
        తిరిగి Py_Pegen_raise_incomplete_ఆదానం(p);
    }
    స్థిరరాశి అక్షరం *bstr = Py_బైట్లు_AS_STRING(tok->బైట్లు);
    PyObject* str = PyUnicode_FromString(bstr);
    ఒకవేళ (str == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Constant(str, ఖాళీ, tok->lineno, tok->నిలువు_స్థానభ్రంశం,
                           tok->చివర_lineno, tok->చివర_నిలువు_స్థానభ్రంశం,
                           p->arena);
}

సమాసం_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    ఒకవేళ (tok->బైట్లు == ఖాళీ) {
        తిరిగి Py_Pegen_raise_incomplete_ఆదానం(p);
    }
    స్థిరరాశి అక్షరం *the_str = Py_బైట్లు_AS_STRING(tok->బైట్లు);
    PyObject *s = _PyPegen_parse_string(p, tok);
    ఒకవేళ (s == ఖాళీ) {
        _Pypegen_raise_decode_error(p);
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        తిరిగి ఖాళీ;
    }
    PyObject *రకం = ఖాళీ;
    ఒకవేళ (the_str && the_str[0] == 'u') {
        రకం = _PyPegen_new_identifier(p, "u");
        ఒకవేళ (రకం == ఖాళీ) {
            తిరిగి ఖాళీ;
        }
    }
    తిరిగి _PyAST_Constant(s, రకం, tok->lineno, tok->నిలువు_స్థానభ్రంశం, tok->చివర_lineno, tok->చివర_నిలువు_స్థానభ్రంశం, p->arena);
}

స్థిర పూర్ణాంకం
_get_interpolation_conversion(Parser *p, Token *debug, ఫలితం_Token_With_Metadata *conversion,
                              ఫలితం_Token_With_Metadata *format)
{
    ఒకవేళ (conversion != ఖాళీ) {
        సమాసం_ty conversion_సమాసం = (సమాసం_ty) conversion->ఫలితం;
        ప్రతిజ్ఞా(conversion_సమాసం->రకం == Name_రకం);
        Py_UCS4 మొదటి = PyUnicode_READ_CHAR(conversion_సమాసం->v.Name.id, 0);
        తిరిగి Py_సురక్షిత_DOWNCAST(మొదటి, Py_UCS4, పూర్ణాంకం);
    }
    లేకపోతే ఒకవేళ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        తిరిగి (పూర్ణాంకం)'r';
    }
    తిరిగి -1;
}

స్థిర PyObject *
strip_interpolation_debug_సమాసం(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_నిడివి(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    అయితే (len > 0) {
        పూర్ణాంకం has_newline = 0;
        అయితే (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            ఒకవేళ (!_PyUnicode_IsWhitespace(c)) {
                ఆపు;
            }
            ఒకవేళ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        ఒకవేళ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            ఆపు;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    ఒకవేళ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        తిరిగి Py_NewRef(exprstr);
    }

    తిరిగి PyUnicode_Substring(exprstr, 0, len - 1);
}

సమాసం_ty _PyPegen_interpolation(Parser *p, సమాసం_ty వ్యక్తీకరణ, Token *debug, ఫలితం_Token_With_Metadata *conversion,
                                 ఫలితం_Token_With_Metadata *format, Token *closing_brace, పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం,
                                 పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena) {

    పూర్ణాంకం conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    పూర్ణాంకం debug_చివర_పంక్తి, debug_చివర_స్థానభ్రంశం;
    PyObject *debug_metadata;
    constant exprstr;

    ఒకవేళ (conversion) {
        debug_చివర_పంక్తి = ((సమాసం_ty) conversion->ఫలితం)->lineno;
        debug_చివర_స్థానభ్రంశం = ((సమాసం_ty) conversion->ఫలితం)->నిలువు_స్థానభ్రంశం;
        debug_metadata = exprstr = conversion->metadata;
    }
    లేకపోతే ఒకవేళ (format) {
        debug_చివర_పంక్తి = ((సమాసం_ty) format->ఫలితం)->lineno;
        debug_చివర_స్థానభ్రంశం = ((సమాసం_ty) format->ఫలితం)->నిలువు_స్థానభ్రంశం + 1;
        debug_metadata = exprstr = format->metadata;
    }
    లేకపోతే {
        debug_చివర_పంక్తి = చివర_lineno;
        debug_చివర_స్థానభ్రంశం = చివర_నిలువు_స్థానభ్రంశం;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    ప్రతిజ్ఞా(exprstr != ఖాళీ);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_సమాసం(exprstr)
        : Py_NewRef(exprstr);
    ఒకవేళ (!final_exprstr || Py_Arena_కూడు_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        తిరిగి ఖాళీ;
    }

    సమాసం_ty interpolation = _PyAST_Interpolation(
        వ్యక్తీకరణ, final_exprstr, conversion_val, format ? (సమాసం_ty) format->ఫలితం : ఖాళీ,
        lineno, నిలువు_స్థానభ్రంశం, చివర_lineno,
        చివర_నిలువు_స్థానభ్రంశం, arena
    );

    ఒకవేళ (!interpolation || !debug) {
        తిరిగి interpolation;
    }

    సమాసం_ty debug_పాఠ్యం = _PyAST_Constant(debug_metadata, ఖాళీ, lineno, నిలువు_స్థానభ్రంశం + 1, debug_చివర_పంక్తి,
                                            debug_చివర_స్థానభ్రంశం - 1, p->arena);
    ఒకవేళ (!debug_పాఠ్యం) {
        తిరిగి ఖాళీ;
    }

    asdl_సమాసం_seq *విలువలు = Py_asdl_సమాసం_seq_new(2, arena);
    ఒకవేళ (!విలువలు) {
        తిరిగి ఖాళీ;
    }
    asdl_seq_సమితి(విలువలు, 0, debug_పాఠ్యం);
    asdl_seq_సమితి(విలువలు, 1, interpolation);
    తిరిగి _PyAST_JoinedStr(విలువలు, lineno, నిలువు_స్థానభ్రంశం, debug_చివర_పంక్తి, debug_చివర_స్థానభ్రంశం, p->arena);
}

సమాసం_ty Py_Pegen_formatted_విలువ(Parser *p, సమాసం_ty వ్యక్తీకరణ, Token *debug, ఫలితం_Token_With_Metadata *conversion,
                                 ఫలితం_Token_With_Metadata *format, Token *closing_brace, పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం,
                                 పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena) {
    పూర్ణాంకం conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    సమాసం_ty formatted_విలువ = Py_AST_Formatted_విలువ(
        వ్యక్తీకరణ, conversion_val, format ? (సమాసం_ty) format->ఫలితం : ఖాళీ,
        lineno, నిలువు_స్థానభ్రంశం, చివర_lineno,
        చివర_నిలువు_స్థానభ్రంశం, arena
    );

    ఒకవేళ (!formatted_విలువ || !debug) {
        తిరిగి formatted_విలువ;
    }

    /* Find the non whitespace token after the "=" */
    పూర్ణాంకం debug_చివర_పంక్తి, debug_చివర_స్థానభ్రంశం;
    PyObject *debug_metadata;

    ఒకవేళ (conversion) {
        debug_చివర_పంక్తి = ((సమాసం_ty) conversion->ఫలితం)->lineno;
        debug_చివర_స్థానభ్రంశం = ((సమాసం_ty) conversion->ఫలితం)->నిలువు_స్థానభ్రంశం;
        debug_metadata = conversion->metadata;
    }
    లేకపోతే ఒకవేళ (format) {
        debug_చివర_పంక్తి = ((సమాసం_ty) format->ఫలితం)->lineno;
        debug_చివర_స్థానభ్రంశం = ((సమాసం_ty) format->ఫలితం)->నిలువు_స్థానభ్రంశం + 1;
        debug_metadata = format->metadata;
    }
    లేకపోతే {
        debug_చివర_పంక్తి = చివర_lineno;
        debug_చివర_స్థానభ్రంశం = చివర_నిలువు_స్థానభ్రంశం;
        debug_metadata = closing_brace->metadata;
    }
    సమాసం_ty debug_పాఠ్యం = _PyAST_Constant(debug_metadata, ఖాళీ, lineno, నిలువు_స్థానభ్రంశం + 1, debug_చివర_పంక్తి,
                                            debug_చివర_స్థానభ్రంశం - 1, p->arena);
    ఒకవేళ (!debug_పాఠ్యం) {
        తిరిగి ఖాళీ;
    }

    asdl_సమాసం_seq *విలువలు = Py_asdl_సమాసం_seq_new(2, arena);
    ఒకవేళ (!విలువలు) {
        తిరిగి ఖాళీ;
    }
    asdl_seq_సమితి(విలువలు, 0, debug_పాఠ్యం);
    asdl_seq_సమితి(విలువలు, 1, formatted_విలువ);
    తిరిగి _PyAST_JoinedStr(విలువలు, lineno, నిలువు_స్థానభ్రంశం, debug_చివర_పంక్తి, debug_చివర_స్థానభ్రంశం, p->arena);
}

స్థిర సమాసం_ty
build_concatenated_బైట్లు(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు, పూర్ణాంకం lineno,
                        పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(అక్షరశ్రేణులు);
    ప్రతిజ్ఞా(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *రకం = asdl_seq_GET(అక్షరశ్రేణులు, 0)->v.Constant.రకం;

    Py_ssize_t మొత్తం = 0;
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        PyObject *బైట్లు = elem->v.Constant.విలువ;
        Py_ssize_t part = Py_బైట్లు_GET_కొలత(బైట్లు);
        ఒకవేళ (part > PY_SSIZE_T_MAX - మొత్తం) {
            Py_Err_No_జ్ఞాపకం();
            తిరిగి ఖాళీ;
        }
        మొత్తం += part;
    }

    Py_బైట్లు_Writer *writer = Py_బైట్లు_Writer_Create(మొత్తం);
    ఒకవేళ (writer == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    అక్షరం *బహిర్గతం = Py_బైట్లు_Writer_Get_దత్తాంశం(writer);

    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        PyObject *బైట్లు = elem->v.Constant.విలువ;
        Py_ssize_t part = Py_బైట్లు_GET_కొలత(బైట్లు);
        ఒకవేళ (part > 0) {
            జ్ఞాపకం_నకలు(బహిర్గతం, Py_బైట్లు_AS_STRING(బైట్లు), part);
            బహిర్గతం += part;
        }
    }

    PyObject *res = Py_బైట్లు_Writer_ముగింపు(writer);
    ఒకవేళ (res == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Constant(res, రకం, lineno, నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, p->arena);
}

స్థిర సమాసం_ty
_build_concatenated_unicode(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు, పూర్ణాంకం lineno,
                        పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(అక్షరశ్రేణులు);
    ప్రతిజ్ఞా(len > 1);

    సమాసం_ty మొదటి = asdl_seq_GET(అక్షరశ్రేణులు, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *రకం = మొదటి->v.Constant.రకం;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    ఒకవేళ (writer == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty current_elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        ప్రతిజ్ఞా(current_elem->రకం == Constant_రకం);

        ఒకవేళ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.విలువ)) {
            PyUnicodeWriter_Discard(writer);
            తిరిగి ఖాళీ;
        }
    }

    PyObject *final = Py_Unicode_Writer_ముగింపు(writer);
    ఒకవేళ (final == ఖాళీ) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_Constant(final, రకం, lineno, నిలువు_స్థానభ్రంశం,
                           చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
}

స్థిర asdl_సమాసం_seq *
_build_concatenated_str(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు,
                               పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                               పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(అక్షరశ్రేణులు);
    ప్రతిజ్ఞా(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        ఎంపిక(elem->రకం) {
            స్థితి Joined_Str_రకం:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.విలువలు);
                ఆపు;
            స్థితి Template_Str_రకం:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.విలువలు);
                ఆపు;
            అప్రమేయ:
                n_flattened_elements++;
                ఆపు;
        }
    }


    asdl_సమాసం_seq* flattened = Py_asdl_సమాసం_seq_new(n_flattened_elements, p->arena);
    ఒకవేళ (flattened == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    కొరకు (Py_ssize_t i = 0; i < len; i++) {
        సమాసం_ty elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        ఎంపిక(elem->రకం) {
            స్థితి Joined_Str_రకం:
                కొరకు (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.విలువలు); j++) {
                    సమాసం_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.విలువలు, j);
                    ఒకవేళ (subvalue == ఖాళీ) {
                        తిరిగి ఖాళీ;
                    }
                    asdl_seq_సమితి(flattened, current_pos++, subvalue);
                }
                ఆపు;
            స్థితి Template_Str_రకం:
                కొరకు (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.విలువలు); j++) {
                    సమాసం_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.విలువలు, j);
                    ఒకవేళ (subvalue == ఖాళీ) {
                        తిరిగి ఖాళీ;
                    }
                    asdl_seq_సమితి(flattened, current_pos++, subvalue);
                }
                ఆపు;
            అప్రమేయ:
                asdl_seq_సమితి(flattened, current_pos++, elem);
                ఆపు;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    పూర్ణాంకం prev_is_constant = 0;
    కొరకు (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        సమాసం_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        ఒకవేళ (elem->రకం == Constant_రకం &&
            PyUnicode_CheckExact(elem->v.Constant.విలువ) &&
            Py_Unicode_GET_నిడివి(elem->v.Constant.విలువ) == 0)
            కొనసాగు;

        ఒకవేళ (!prev_is_constant || elem->రకం != Constant_రకం) {
            n_elements++;
        }
        prev_is_constant = elem->రకం == Constant_రకం;
    }

    asdl_సమాసం_seq* విలువలు = Py_asdl_సమాసం_seq_new(n_elements, p->arena);
    ఒకవేళ (విలువలు == ఖాళీ) {
        తిరిగి ఖాళీ;
    }

    /* build folded list */
    current_pos = 0;
    కొరకు (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        సమాసం_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        ఒకవేళ (elem->రకం == Constant_రకం) {
            ఒకవేళ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->రకం == Constant_రకం) {
                సమాసం_ty మొదటి_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *రకం = elem->v.Constant.రకం;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                ఒకవేళ (writer == ఖాళీ) {
                    తిరిగి ఖాళీ;
                }
                సమాసం_ty చివరి_elem = elem;
                Py_ssize_t j;
                కొరకు (j = i; j < n_flattened_elements; j++) {
                    సమాసం_ty current_elem = asdl_seq_GET(flattened, j);
                    ఒకవేళ (current_elem->రకం == Constant_రకం) {
                        ఒకవేళ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.విలువ)) {
                            PyUnicodeWriter_Discard(writer);
                            తిరిగి ఖాళీ;
                        }
                        చివరి_elem = current_elem;
                    } లేకపోతే {
                        ఆపు;
                    }
                }
                i = j - 1;

                PyObject *concat_str = Py_Unicode_Writer_ముగింపు(writer);
                ఒకవేళ (concat_str == ఖాళీ) {
                    తిరిగి ఖాళీ;
                }
                ఒకవేళ (Py_Arena_కూడు_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    తిరిగి ఖాళీ;
                }
                elem = _PyAST_Constant(concat_str, రకం, మొదటి_elem->lineno,
                                       మొదటి_elem->నిలువు_స్థానభ్రంశం,
                                       చివరి_elem->చివర_lineno,
                                       చివరి_elem->చివర_నిలువు_స్థానభ్రంశం, p->arena);
                ఒకవేళ (elem == ఖాళీ) {
                    తిరిగి ఖాళీ;
                }
            }

            /* Drop all empty contanst strings */
            ఒకవేళ (PyUnicode_CheckExact(elem->v.Constant.విలువ) &&
                Py_Unicode_GET_నిడివి(elem->v.Constant.విలువ) == 0) {
                కొనసాగు;
            }
        }

        asdl_seq_సమితి(విలువలు, current_pos++, elem);
    }

    ప్రతిజ్ఞా(current_pos == n_elements);
    తిరిగి విలువలు;
}

స్థిర సమాసం_ty
_build_concatenated_joined_str(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు,
                               పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                               పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena)
{
    asdl_సమాసం_seq *విలువలు = _build_concatenated_str(p, అక్షరశ్రేణులు, lineno,
        నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
    ఒకవేళ (!విలువలు) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_JoinedStr(విలువలు, lineno, నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, p->arena);
}

సమాసం_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు,
                               పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                               పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena)
{
    asdl_సమాసం_seq *విలువలు = _build_concatenated_str(p, అక్షరశ్రేణులు, lineno,
        నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
    ఒకవేళ (!విలువలు) {
        తిరిగి ఖాళీ;
    }
    తిరిగి _PyAST_TemplateStr(విలువలు, lineno, నిలువు_స్థానభ్రంశం, చివర_lineno,
        చివర_నిలువు_స్థానభ్రంశం, arena);
}

సమాసం_ty
Py_Pegen_concatenate_అక్షరశ్రేణులు(Parser *p, asdl_సమాసం_seq *అక్షరశ్రేణులు,
                             పూర్ణాంకం lineno, పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno,
                             పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(అక్షరశ్రేణులు);
    ప్రతిజ్ఞా(len > 0);

    పూర్ణాంకం f_string_దొరికింది = 0;
    పూర్ణాంకం unicode_string_దొరికింది = 0;
    పూర్ణాంకం బైట్లు_దొరికింది = 0;

    Py_ssize_t i = 0;
    కొరకు (i = 0; i < len; i++) {
        సమాసం_ty elem = asdl_seq_GET(అక్షరశ్రేణులు, i);
        ఎంపిక(elem->రకం) {
            స్థితి Constant_రకం:
                ఒకవేళ (Py_బైట్లు_Check_Exact(elem->v.Constant.విలువ)) {
                    బైట్లు_దొరికింది = 1;
                } లేకపోతే {
                    unicode_string_దొరికింది = 1;
                }
                ఆపు;
            స్థితి Joined_Str_రకం:
                f_string_దొరికింది = 1;
                ఆపు;
            స్థితి Template_Str_రకం:
                // python.gram handles this; we should never get here
                ప్రతిజ్ఞా(0);
                ఆపు;
            అప్రమేయ:
                f_string_దొరికింది = 1;
                ఆపు;
        }
    }

    // Cannot mix unicode and bytes
    ఒకవేళ ((unicode_string_దొరికింది || f_string_దొరికింది) && బైట్లు_దొరికింది) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        తిరిగి ఖాళీ;
    }

    // If it's only bytes or only unicode string, do a simple concat
    ఒకవేళ (!f_string_దొరికింది) {
        ఒకవేళ (len == 1) {
            తిరిగి asdl_seq_GET(అక్షరశ్రేణులు, 0);
        }
        లేకపోతే ఒకవేళ (బైట్లు_దొరికింది) {
            తిరిగి build_concatenated_బైట్లు(p, అక్షరశ్రేణులు, lineno, నిలువు_స్థానభ్రంశం,
                చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
        }
        లేకపోతే {
            తిరిగి _build_concatenated_unicode(p, అక్షరశ్రేణులు, lineno, నిలువు_స్థానభ్రంశం,
                చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
        }
    }

    తిరిగి _build_concatenated_joined_str(p, అక్షరశ్రేణులు, lineno,
        నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
}

స్థిర పూర్ణాంకం
warn_relative_import_యొక్క_lazy(Parser *p, asdl_seq *dots, సమాసం_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    ఒకవేళ (p->call_invalid_నియమాలు) {
        తిరిగి 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *చివరి_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    ఒకవేళ (
        చివరి_dot->చివర_lineno == module->lineno
        && చివరి_dot->చివర_నిలువు_స్థానభ్రంశం == module->నిలువు_స్థానభ్రంశం
    ) {
        తిరిగి 0;
    }

    పూర్ణాంకం లెక్క = Py_Pegen_seq_లెక్క_dots(dots);
    అక్షరం *buf = PyMem_RawMalloc(లెక్క + 1);
    ఒకవేళ (buf == ఖాళీ) {
        Py_Err_No_జ్ఞాపకం();
        తిరిగి -1;
    }
    memset(buf, '.', లెక్క);
    buf[లెక్క] = '\0';

    PyObject *సందేశం = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    ఒకవేళ (సందేశం == ఖాళీ) {
        తిరిగి -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    పూర్ణాంకం res = _PyErr_EmitSyntaxWarning(సందేశం,
                                       info.filename,
                                       module->lineno,
                                       module->నిలువు_స్థానభ్రంశం + 1,
                                       module->చివర_lineno,
                                       module->చివర_నిలువు_స్థానభ్రంశం + 1,
                                       info.module);
    Py_DECREF(సందేశం);
    తిరిగి res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, సమాసం_ty module_name,
                             asdl_alias_seq *పేర్లు, సమాసం_ty lazy_token, పూర్ణాంకం lineno,
                             పూర్ణాంకం నిలువు_స్థానభ్రంశం, పూర్ణాంకం చివర_lineno, పూర్ణాంకం చివర_నిలువు_స్థానభ్రంశం,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    పూర్ణాంకం level = Py_Pegen_seq_లెక్క_dots(dots);
    ఒకవేళ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        ఒకవేళ (lazy_token) {
            RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            తిరిగి ఖాళీ;
        }
        కొరకు (Py_ssize_t i = 0; i < asdl_seq_LEN(పేర్లు); i++) {
            alias_ty alias = asdl_seq_GET(పేర్లు, i);
            ఒకవేళ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    లేకపోతే ఒకవేళ (
        level > 0
        && lazy_token == ఖాళీ
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        ఒకవేళ (warn_relative_import_యొక్క_lazy(p, dots, module_name) < 0) {
            తిరిగి ఖాళీ;
        }
    }
    తిరిగి _PyAST_ImportFrom(module, పేర్లు, level, lazy_token ? 1 : 0, lineno,
                             నిలువు_స్థానభ్రంశం, చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, arena);
}

asdl_stmt_seq*
Py_Pegen_రిజిస్టరు_stmts(Parser *p, asdl_stmt_seq* stmts) {
    ఒకవేళ (!p->call_invalid_నియమాలు) {
        తిరిగి stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    ఒకవేళ (len == 0) {
        తిరిగి stmts;
    }
    stmt_ty చివరి_stmt = asdl_seq_GET(stmts, len - 1);
    ఒకవేళ (p->చివరి_stmt_location.lineno > చివరి_stmt->lineno) {
        తిరిగి stmts;
    }
    p->చివరి_stmt_location.lineno = చివరి_stmt->lineno;
    p->చివరి_stmt_location.నిలువు_స్థానభ్రంశం = చివరి_stmt->నిలువు_స్థానభ్రంశం;
    p->చివరి_stmt_location.చివర_lineno = చివరి_stmt->చివర_lineno;
    p->చివరి_stmt_location.చివర_నిలువు_స్థానభ్రంశం = చివరి_stmt->చివర_నిలువు_స్థానభ్రంశం;
    తిరిగి stmts;
}
