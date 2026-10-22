#अन्तर्भाव <Python.h>
#अन्तर्भाव "pycore_pystate.h"         // _PyInterpreterState_GET()
#अन्तर्भाव "pycore_runtime.h"         // _PyRuntime
#अन्तर्भाव "pycore_unicodeobject.h"   // _PyUnicode_InternImmortal()
#अन्तर्भाव "pycore_pyerrors.h"        // _PyErr_EmitSyntaxWarning()

#अन्तर्भाव "pegen.h"
#अन्तर्भाव "string_parser.h"          // _PyPegen_decode_string()
#अन्तर्भाव "lexer/state.h"            // tok_state


शून्य *
_PyPegen_dummy_name(Parser *p, ...)
{
    प्रत्यागम &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    यदि (!seq) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_समुच्चयः_UNTYPED(seq, 0, a);
    प्रत्यागम seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_निवेशय_अन्तर्_front(Parser *p, शून्य *a, asdl_seq *seq)
{
    प्रतिज्ञा(a != रिक्त);
    यदि (!seq) {
        प्रत्यागम _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }

    asdl_seq_समुच्चयः_UNTYPED(new_seq, 0, a);
    प्रति (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_समुच्चयः_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    प्रत्यागम new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
Py_Pegen_seq_append_गन्तव्यम्_अन्तः(Parser *p, asdl_seq *seq, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    यदि (!seq) {
        प्रत्यागम _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }

    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_समुच्चयः_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_समुच्चयः_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    प्रत्यागम new_seq;
}

स्थिर Py_ssize_t
get_flattened_seq_आकार(asdl_seq *seqs)
{
    Py_ssize_t आकार = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        आकार += asdl_seq_LEN(inner_seq);
    }
    प्रत्यागम आकार;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_आकार = get_flattened_seq_आकार(seqs);
    प्रतिज्ञा(flattened_seq_आकार > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_आकार, p->arena);
    यदि (!flattened_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क flattened_seq_idx = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        प्रति (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_समुच्चयः_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    प्रतिज्ञा(flattened_seq_idx == flattened_seq_आकार);

    प्रत्यागम flattened_seq;
}

शून्य *
Py_Pegen_seq_चरमः_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रत्यागम asdl_seq_GET_UNTYPED(seq, len - 1);
}

शून्य *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    प्रत्यागम asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
व्यञ्जकम्_ty
Py_Pegen_join_नामानि_with_dot(Parser *p, व्यञ्जकम्_ty first_name, व्यञ्जकम्_ty second_name)
{
    प्रतिज्ञा(first_name != रिक्त && second_name != रिक्त);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    यदि (!uni) {
        प्रत्यागम रिक्त;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, uni) < 0) {
        Py_DECREF(uni);
        प्रत्यागम रिक्त;
    }

    प्रत्यागम _PyAST_Name(uni, आरोपय, EXTRA_व्यञ्जकम्(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
पूर्णाङ्क
Py_Pegen_seq_गणनम्_dots(asdl_seq *seq)
{
    पूर्णाङ्क सङ्ख्या_सम्बन्धि_dots = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_व्यञ्जकम् = asdl_seq_GET_UNTYPED(seq, i);
        विकल्प (current_व्यञ्जकम्->type) {
            पक्ष ELLIPSIS:
                सङ्ख्या_सम्बन्धि_dots += 3;
                भङ्ग;
            पक्ष DOT:
                सङ्ख्या_सम्बन्धि_dots += 1;
                भङ्ग;
            स्वभाव:
                Py_UNREACHABLE();
        }
    }

    प्रत्यागम सङ्ख्या_सम्बन्धि_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
Py_Pegen_alias_for_तारकम्(Parser *p, पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                        पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    यदि (!str) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_alias(str, रिक्त, lineno, col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
Py_Pegen_map_नामानि_गन्तव्यम्_ids(Parser *p, asdl_व्यञ्जकम्_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty e = asdl_seq_GET(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, e->v.Name.id);
    }
    प्रत्यागम new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_व्यञ्जकम्_Pair *
Py_Pegen_cmpop_व्यञ्जकम्_pair(Parser *p, cmpop_ty cmpop, व्यञ्जकम्_ty व्यञ्जकम्)
{
    प्रतिज्ञा(व्यञ्जकम् != रिक्त);
    Cmpop_व्यञ्जकम्_Pair *a = _PyArena_Malloc(p->arena, परिमाण(Cmpop_व्यञ्जकम्_Pair));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->cmpop = cmpop;
    a->व्यञ्जकम् = व्यञ्जकम्;
    प्रत्यागम a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_व्यञ्जकम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->cmpop);
    }
    प्रत्यागम new_seq;
}

asdl_व्यञ्जकम्_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_व्यञ्जकम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->व्यञ्जकम्);
    }
    प्रत्यागम new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
स्थिर asdl_व्यञ्जकम्_seq *
समुच्चयः_seq_context(Parser *p, asdl_व्यञ्जकम्_seq *seq, व्यञ्जकम्_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    यदि (len == 0) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty e = asdl_seq_GET(seq, i);
        व्यञ्जकम्_ty new_e = Py_Pegen_समुच्चयः_व्यञ्जकम्_context(p, e, ctx);
        यदि (!new_e) {
            प्रत्यागम रिक्त;
        }
        asdl_seq_समुच्चयः(new_seq, i, new_e);
    }
    प्रत्यागम new_seq;
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_name_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Name(e->v.Name.id, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_tuple_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    asdl_व्यञ्जकम्_seq *seq = समुच्चयः_seq_context(p, e->v.Tuple.elts, ctx);
    यदि (!seq && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Tuple(seq, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_list_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    asdl_व्यञ्जकम्_seq *seq = समुच्चयः_seq_context(p, e->v.List.elts, ctx);
    यदि (!seq && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_List(seq, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_subscript_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Subscript(e->v.Subscript.मानम्, e->v.Subscript.खण्डिका,
                            ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_attribute_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Attribute(e->v.Attribute.मानम्, e->v.Attribute.attr,
                            ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
समुच्चयः_starred_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    व्यञ्जकम्_ty inner = Py_Pegen_समुच्चयः_व्यञ्जकम्_context(p, e->v.Starred.मानम्, ctx);
    यदि (!inner) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Starred(inner, ctx, EXTRA_व्यञ्जकम्(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
व्यञ्जकम्_ty
Py_Pegen_समुच्चयः_व्यञ्जकम्_context(Parser *p, व्यञ्जकम्_ty व्यञ्जकम्, व्यञ्जकम्_context_ty ctx)
{
    प्रतिज्ञा(व्यञ्जकम् != रिक्त);

    व्यञ्जकम्_ty new = रिक्त;
    विकल्प (व्यञ्जकम्->विधा) {
        पक्ष Name_विधा:
            new = समुच्चयः_name_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Tuple_विधा:
            new = समुच्चयः_tuple_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष List_विधा:
            new = समुच्चयः_list_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Subscript_विधा:
            new = समुच्चयः_subscript_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Attribute_विधा:
            new = समुच्चयः_attribute_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Starred_विधा:
            new = समुच्चयः_starred_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        स्वभाव:
            new = व्यञ्जकम्;
    }
    प्रत्यागम new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
व्यञ्जकम्_ty
_PyPegen_make_target(Parser *p, व्यञ्जकम्_ty व्यञ्जकम्, TARGETS_TYPE targets_type)
{
    प्रतिज्ञा(व्यञ्जकम् != रिक्त);
    प्रतिज्ञा(targets_type != FOR_TARGETS);
    यदि (_PyPegen_get_invalid_target(व्यञ्जकम्, targets_type) != रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम Py_Pegen_समुच्चयः_व्यञ्जकम्_context(
        p, व्यञ्जकम्, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
कुञ्चिका_मानम्_Pair *
Py_Pegen_कुञ्चिका_मानम्_pair(Parser *p, व्यञ्जकम्_ty कुञ्चिका, व्यञ्जकम्_ty मानम्)
{
    कुञ्चिका_मानम्_Pair *a = _PyArena_Malloc(p->arena, परिमाण(कुञ्चिका_मानम्_Pair));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->कुञ्चिका = कुञ्चिका;
    a->मानम् = मानम्;
    प्रत्यागम a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_व्यञ्जकम्_seq *
Py_Pegen_get_कुञ्चिकाः(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_मानम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->कुञ्चिका);
    }
    प्रत्यागम new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_व्यञ्जकम्_seq *
Py_Pegen_get_मानानि(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_मानम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->मानम्);
    }
    प्रत्यागम new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
कुञ्चिका_आदर्शः_Pair *
Py_Pegen_कुञ्चिका_आदर्शः_pair(Parser *p, व्यञ्जकम्_ty कुञ्चिका, आदर्शः_ty आदर्शः)
{
    कुञ्चिका_आदर्शः_Pair *a = _PyArena_Malloc(p->arena, परिमाण(कुञ्चिका_आदर्शः_Pair));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->कुञ्चिका = कुञ्चिका;
    a->आदर्शः = आदर्शः;
    प्रत्यागम a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_व्यञ्जकम्_seq *
Py_Pegen_get_आदर्शः_कुञ्चिकाः(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_आदर्शः_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->कुञ्चिका);
    }
    प्रत्यागम new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_आदर्शः_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_आदर्शः_seq *new_seq = Py_asdl_आदर्शः_seq_new(len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_आदर्शः_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_समुच्चयः(new_seq, i, pair->आदर्शः);
    }
    प्रत्यागम new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, व्यञ्जकम्_ty मानम्, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, परिमाण(NameDefaultPair));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->arg = Py_Pegen_सङ्कलय_type_comment_गन्तव्यम्_arg(p, arg, tc);
    यदि (!a->arg) {
        प्रत्यागम रिक्त;
    }
    a->मानम् = मानम्;
    प्रत्यागम a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_नामानि, asdl_seq *नामानि_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, परिमाण(SlashWithDefault));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->plain_नामानि = plain_नामानि;
    a->नामानि_with_defaults = नामानि_with_defaults;
    प्रत्यागम a;
}

/* Constructs a StarEtc */
तारकम्_Etc *
Py_Pegen_तारकम्_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    तारकम्_Etc *a = _PyArena_Malloc(p->arena, परिमाण(तारकम्_Etc));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    प्रत्यागम a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क k = 0;
    प्रति (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_समुच्चयः_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    प्रति (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_समुच्चयः_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    प्रत्यागम new_seq;
}

स्थिर asdl_arg_seq*
get_नामानि(Parser *p, asdl_seq *नामानि_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(नामानि_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    यदि (!seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(नामानि_with_defaults, i);
        asdl_seq_समुच्चयः(seq, i, pair->arg);
    }
    प्रत्यागम seq;
}

स्थिर asdl_व्यञ्जकम्_seq *
_get_defaults(Parser *p, asdl_seq *नामानि_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(नामानि_with_defaults);
    asdl_व्यञ्जकम्_seq *seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    यदि (!seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(नामानि_with_defaults, i);
        asdl_seq_समुच्चयः(seq, i, pair->मानम्);
    }
    प्रत्यागम seq;
}

स्थिर पूर्णाङ्क
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    यदि (slash_without_default != रिक्त) {
        *posonlyargs = slash_without_default;
    }
    अन्यथा यदि (slash_with_default != रिक्त) {
        asdl_arg_seq *slash_with_default_नामानि =
                get_नामानि(p, slash_with_default->नामानि_with_defaults);
        यदि (!slash_with_default_नामानि) {
            प्रत्यागम -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_नामानि,
                (asdl_seq*)slash_with_default_नामानि);
    }
    अन्यथा {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    प्रत्यागम *posonlyargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posargs(Parser *p,
              asdl_arg_seq *plain_नामानि,
              asdl_seq *नामानि_with_default,
              asdl_arg_seq **posargs) {

    यदि (नामानि_with_default != रिक्त) {
        यदि (plain_नामानि != रिक्त) {
            asdl_arg_seq *नामानि_with_default_नामानि = get_नामानि(p, नामानि_with_default);
            यदि (!नामानि_with_default_नामानि) {
                प्रत्यागम -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_नामानि, (asdl_seq*)नामानि_with_default_नामानि);
        }
        अन्यथा {
            *posargs = get_नामानि(p, नामानि_with_default);
        }
    }
    अन्यथा {
        यदि (plain_नामानि != रिक्त) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            प्रतिज्ञा(0);
            *posargs = plain_नामानि;
        }
        अन्यथा {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    प्रत्यागम *posargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *नामानि_with_default,
                  asdl_व्यञ्जकम्_seq **posdefaults) {
    यदि (slash_with_default != रिक्त && नामानि_with_default != रिक्त) {
        asdl_व्यञ्जकम्_seq *slash_with_default_मानानि =
                _get_defaults(p, slash_with_default->नामानि_with_defaults);
        यदि (!slash_with_default_मानानि) {
            प्रत्यागम -1;
        }
        asdl_व्यञ्जकम्_seq *नामानि_with_default_मानानि = _get_defaults(p, नामानि_with_default);
        यदि (!नामानि_with_default_मानानि) {
            प्रत्यागम -1;
        }
        *posdefaults = (asdl_व्यञ्जकम्_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_मानानि,
                (asdl_seq*)नामानि_with_default_मानानि);
    }
    अन्यथा यदि (slash_with_default == रिक्त && नामानि_with_default != रिक्त) {
        *posdefaults = _get_defaults(p, नामानि_with_default);
    }
    अन्यथा यदि (slash_with_default != रिक्त && नामानि_with_default == रिक्त) {
        *posdefaults = _get_defaults(p, slash_with_default->नामानि_with_defaults);
    }
    अन्यथा {
        *posdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    }
    प्रत्यागम *posdefaults == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_kwargs(Parser *p, तारकम्_Etc *तारकम्_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_व्यञ्जकम्_seq **kwdefaults) {
    यदि (तारकम्_etc != रिक्त && तारकम्_etc->kwonlyargs != रिक्त) {
        *kwonlyargs = get_नामानि(p, तारकम्_etc->kwonlyargs);
    }
    अन्यथा {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    यदि (*kwonlyargs == रिक्त) {
        प्रत्यागम -1;
    }

    यदि (तारकम्_etc != रिक्त && तारकम्_etc->kwonlyargs != रिक्त) {
        *kwdefaults = _get_defaults(p, तारकम्_etc->kwonlyargs);
    }
    अन्यथा {
        *kwdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    }

    यदि (*kwdefaults == रिक्त) {
        प्रत्यागम -1;
    }

    प्रत्यागम 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_नामानि,
                        asdl_seq *नामानि_with_default, तारकम्_Etc *तारकम्_etc)
{
    asdl_arg_seq *posonlyargs;
    यदि (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        प्रत्यागम रिक्त;
    }

    asdl_arg_seq *posargs;
    यदि (_make_posargs(p, plain_नामानि, नामानि_with_default, &posargs) == -1) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *posdefaults;
    यदि (_make_posdefaults(p,slash_with_default, नामानि_with_default, &posdefaults) == -1) {
        प्रत्यागम रिक्त;
    }

    arg_ty vararg = रिक्त;
    यदि (तारकम्_etc != रिक्त && तारकम्_etc->vararg != रिक्त) {
        vararg = तारकम्_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_व्यञ्जकम्_seq *kwdefaults;
    यदि (_make_kwargs(p, तारकम्_etc, &kwonlyargs, &kwdefaults) == -1) {
        प्रत्यागम रिक्त;
    }

    arg_ty kwarg = रिक्त;
    यदि (तारकम्_etc != रिक्त && तारकम्_etc->kwarg != रिक्त) {
        kwarg = तारकम्_etc->kwarg;
    }

    प्रत्यागम _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
Py_Pegen_रिक्तानि_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!posonlyargs) {
        प्रत्यागम रिक्त;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!posargs) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *posdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    यदि (!posdefaults) {
        प्रत्यागम रिक्त;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!kwonlyargs) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *kwdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    यदि (!kwdefaults) {
        प्रत्यागम रिक्त;
    }

    प्रत्यागम _PyAST_arguments(posonlyargs, posargs, रिक्त, kwonlyargs,
                            kwdefaults, रिक्त, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty विधा)
{
    AugOperator *a = _PyArena_Malloc(p->arena, परिमाण(AugOperator));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->विधा = विधा;
    प्रत्यागम a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_व्यञ्जकम्_seq *decorators, stmt_ty function_def)
{
    प्रतिज्ञा(function_def != रिक्त);
    यदि (function_def->विधा == Async_Function_Def_विधा) {
        प्रत्यागम _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.तर्काः,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_विस्थापनम्,
            function_def->अन्तः_lineno, function_def->अन्तः_col_विस्थापनम्, p->arena);
    }

    प्रत्यागम _PyAST_FunctionDef(
        function_def->v.FunctionDef.name,
        function_def->v.FunctionDef.तर्काः,
        function_def->v.FunctionDef.body, decorators,
        function_def->v.FunctionDef.returns,
        function_def->v.FunctionDef.type_comment,
        function_def->v.FunctionDef.type_params,
        function_def->lineno, function_def->col_विस्थापनम्,
        function_def->अन्तः_lineno, function_def->अन्तः_col_विस्थापनम्, p->arena);
}

/* Construct a ClassDef equivalent to class_def, but with decorators */
stmt_ty
_PyPegen_class_def_decorators(Parser *p, asdl_व्यञ्जकम्_seq *decorators, stmt_ty class_def)
{
    प्रतिज्ञा(class_def != रिक्त);
    प्रत्यागम _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_विस्थापनम्, class_def->अन्तः_lineno,
        class_def->अन्तः_col_विस्थापनम्, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, शून्य *element, पूर्णाङ्क is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, परिमाण(KeywordOrStarred));
    यदि (!a) {
        प्रत्यागम रिक्त;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    प्रत्यागम a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
स्थिर पूर्णाङ्क
seq_सङ्ख्या_सम्बन्धि_starred_exprs(asdl_seq *seq)
{
    पूर्णाङ्क n = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        यदि (!k->is_keyword) {
            n++;
        }
    }
    प्रत्यागम n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_व्यञ्जकम्_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    पूर्णाङ्क new_len = seq_सङ्ख्या_सम्बन्धि_starred_exprs(kwargs);
    यदि (new_len == 0) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(new_len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क idx = 0;
    प्रति (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        यदि (!k->is_keyword) {
            asdl_seq_समुच्चयः(new_seq, idx++, k->element);
        }
    }
    प्रत्यागम new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_सङ्ख्या_सम्बन्धि_starred_exprs(kwargs);
    यदि (new_len == 0) {
        प्रत्यागम रिक्त;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    यदि (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क idx = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        यदि (k->is_keyword) {
            asdl_seq_समुच्चयः(new_seq, idx++, k->element);
        }
    }
    प्रत्यागम new_seq;
}

व्यञ्जकम्_ty
_PyPegen_ensure_imaginary(Parser *p, व्यञ्जकम्_ty घातांकः)
{
    यदि (घातांकः->विधा != Constant_विधा || !PyComplex_CheckExact(घातांकः->v.Constant.मानम्)) {
        RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(घातांकः, "imaginary number required in complex literal");
        प्रत्यागम रिक्त;
    }
    प्रत्यागम घातांकः;
}

व्यञ्जकम्_ty
_PyPegen_ensure_real(Parser *p, व्यञ्जकम्_ty घातांकः)
{
    यदि (घातांकः->विधा != Constant_विधा || PyComplex_CheckExact(घातांकः->v.Constant.मानम्)) {
        RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(घातांकः, "real number required in complex literal");
        प्रत्यागम रिक्त;
    }
    प्रत्यागम घातांकः;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = रिक्त;
    Py_ssize_t अङ्कः = p->type_ignore_comments.अङ्कः_वस्तूनि;
    यदि (अङ्कः > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(अङ्कः, p->arena);
        यदि (type_ignores == रिक्त) {
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < अङ्कः; i++) {
            PyObject *चिह्नकम् = _PyPegen_new_type_comment(p, p->type_ignore_comments.वस्तूनि[i].comment);
            यदि (चिह्नकम् == रिक्त) {
                प्रत्यागम रिक्त;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.वस्तूनि[i].lineno,
                                                  चिह्नकम्, p->arena);
            यदि (ti == रिक्त) {
                प्रत्यागम रिक्त;
            }
            asdl_seq_समुच्चयः(type_ignores, i, ti);
        }
    }
    प्रत्यागम _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ध्रुव वर्ण *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, वाक्यदैर्घ्यम्(s), रिक्त);
    यदि (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, res) < 0) {
        Py_DECREF(res);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम res;
}

arg_ty
Py_Pegen_सङ्कलय_type_comment_गन्तव्यम्_arg(Parser *p, arg_ty a, Token *tc)
{
    यदि (tc == रिक्त) {
        प्रत्यागम a;
    }
    ध्रुव वर्ण *अष्टकानि = Py_अष्टकानि_As_String(tc->अष्टकानि);
    यदि (अष्टकानि == रिक्त) {
        प्रत्यागम रिक्त;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, अष्टकानि);
    यदि (tco == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_विस्थापनम्, a->अन्तः_lineno, a->अन्तः_col_विस्थापनम्,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
पूर्णाङ्क
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    प्रतिज्ञा(t->अष्टकानि != रिक्त);
    प्रतिज्ञा(t->type == NOTEQUAL);

    ध्रुव वर्ण* tok_str = Py_अष्टकानि_AS_STRING(t->अष्टकानि);
    यदि (p->flags & PyPARSE_BARRY_AS_BDFL && वाक्यं_तुलय(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        प्रत्यागम -1;
    }
    यदि (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        प्रत्यागम वाक्यं_तुलय(tok_str, "!=");
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyPegen_check_legacy_stmt(Parser *p, व्यञ्जकम्_ty name) {
    यदि (name->विधा != Name_विधा) {
        प्रत्यागम 0;
    }
    ध्रुव वर्ण* candidates[2] = {"print", "exec"};
    प्रति (पूर्णाङ्क i=0; i<2; i++) {
        यदि (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            प्रत्यागम 1;
        }
    }
    प्रत्यागम 0;
}

शून्य *
_PyPegen_raise_error_for_missing_comma(Parser *p, व्यञ्जकम्_ty a, व्यञ्जकम्_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    यदि (_PyPegen_check_legacy_stmt(p, a)) {
        प्रत्यागम रिक्त;
    }
    // Only raise inside parentheses/brackets (level > 0)
    यदि (p->tokens[p->mark - 1]->level == 0) {
        प्रत्यागम रिक्त;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    यदि (a->अन्तः_lineno > a->lineno) {
        प्रत्यागम RAISE_ERROR_ज्ञातम्_LOCATION(
            p, PyExc_SyntaxError, a->अन्तः_lineno, a->col_विस्थापनम्,
            a->अन्तः_lineno, a->अन्तः_col_विस्थापनम्,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    प्रत्यागम RAISE_ERROR_ज्ञातम्_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_विस्थापनम्,
        b->अन्तः_lineno, b->अन्तः_col_विस्थापनम्,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

स्थिर परिणाम_Token_With_Metadata *
परिणाम_token_with_metadata(Parser *p, शून्य *परिणाम, PyObject *metadata)
{
    परिणाम_Token_With_Metadata *res = _PyArena_Malloc(p->arena, परिमाण(परिणाम_Token_With_Metadata));
    यदि (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    res->metadata = metadata;
    res->परिणाम = परिणाम;
    प्रत्यागम res;
}

स्थिर वर्ण
formatted_string_उपसर्गः(ध्रुव Parser *p)
{
    पूर्णाङ्क nested = 0;
    प्रति (पूर्णाङ्क i = p->mark - 1; i >= 0; i--) {
        पूर्णाङ्क type = p->tokens[i]->type;
        यदि (type == FSTRING_अन्तः || type == TSTRING_अन्तः) {
            nested++;
        }
        अन्यथा यदि (type == FSTRING_START || type == TSTRING_START) {
            यदि (nested == 0) {
                प्रत्यागम type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

परिणाम_Token_With_Metadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, व्यञ्जकम्_ty conv)
{
    यदि (conv_token->lineno != conv->lineno || conv_token->अन्तः_col_विस्थापनम् != conv->col_विस्थापनम्) {
        प्रत्यागम RAISE_SYNTAX_ERROR_ज्ञातम्_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_उपसर्गः(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    यदि (Py_Unicode_GET_दीर्घता(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_उपसर्गः(p),
                                            conv->v.Name.id);
        प्रत्यागम रिक्त;
    }

    प्रत्यागम परिणाम_token_with_metadata(p, conv, conv_token->metadata);
}

परिणाम_Token_With_Metadata *
Py_Pegen_सज्जता_पूर्णम्_format_spec(Parser *p, Token *colon, asdl_व्यञ्जकम्_seq *spec, पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्,
                                पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena)
{
    यदि (!spec) {
        प्रत्यागम रिक्त;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_वस्तूनि = asdl_seq_LEN(spec);
    Py_ssize_t non_रिक्तानि_गणनम् = 0;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(spec, i);
        non_रिक्तानि_गणनम् += !(item->विधा == Constant_विधा &&
                             PyUnicode_CheckExact(item->v.Constant.मानम्) &&
                             Py_Unicode_GET_दीर्घता(item->v.Constant.मानम्) == 0);
    }
    यदि (non_रिक्तानि_गणनम् != n_वस्तूनि) {
        asdl_व्यञ्जकम्_seq *resized_spec =
            Py_asdl_व्यञ्जकम्_seq_new(non_रिक्तानि_गणनम्, p->arena);
        यदि (resized_spec == रिक्त) {
            प्रत्यागम रिक्त;
        }
        Py_ssize_t j = 0;
        प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
            व्यञ्जकम्_ty item = asdl_seq_GET(spec, i);
            यदि (item->विधा == Constant_विधा &&
                PyUnicode_CheckExact(item->v.Constant.मानम्) &&
                Py_Unicode_GET_दीर्घता(item->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
            asdl_seq_समुच्चयः(resized_spec, j++, item);
        }
        प्रतिज्ञा(j == non_रिक्तानि_गणनम्);
        spec = resized_spec;
    }
    व्यञ्जकम्_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    यदि (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->विधा == Constant_विधा)) {
        res = _PyAST_JoinedStr(spec, lineno, col_विस्थापनम्, अन्तः_lineno,
                                    अन्तः_col_विस्थापनम्, p->arena);
    } अन्यथा {
        res = Py_Pegen_concatenate_वाक्यानि(p, spec,
                             lineno, col_विस्थापनम्, अन्तः_lineno,
                             अन्तः_col_विस्थापनम्, arena);
    }
    यदि (!res) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम परिणाम_token_with_metadata(p, res, colon->metadata);
}

ध्रुव वर्ण *
Py_Pegen_get_व्यञ्जकम्_name(व्यञ्जकम्_ty e)
{
    प्रतिज्ञा(e != रिक्त);
    विकल्प (e->विधा) {
        पक्ष Attribute_विधा:
            प्रत्यागम "attribute";
        पक्ष Subscript_विधा:
            प्रत्यागम "subscript";
        पक्ष Starred_विधा:
            प्रत्यागम "starred";
        पक्ष Name_विधा:
            प्रत्यागम "name";
        पक्ष List_विधा:
            प्रत्यागम "list";
        पक्ष Tuple_विधा:
            प्रत्यागम "tuple";
        पक्ष Lambda_विधा:
            प्रत्यागम "lambda";
        पक्ष Call_विधा:
            प्रत्यागम "function call";
        पक्ष Bool_Op_विधा:
        पक्ष Bin_Op_विधा:
        पक्ष Unary_Op_विधा:
            प्रत्यागम "expression";
        पक्ष Generator_Exp_विधा:
            प्रत्यागम "generator expression";
        पक्ष Yield_विधा:
        पक्ष Yield_From_विधा:
            प्रत्यागम "yield expression";
        पक्ष Await_विधा:
            प्रत्यागम "await expression";
        पक्ष List_Comp_विधा:
            प्रत्यागम "list comprehension";
        पक्ष समुच्चयः_Comp_विधा:
            प्रत्यागम "set comprehension";
        पक्ष Dict_Comp_विधा:
            प्रत्यागम "dict comprehension";
        पक्ष Dict_विधा:
            प्रत्यागम "dict literal";
        पक्ष समुच्चयः_विधा:
            प्रत्यागम "set display";
        पक्ष Joined_Str_विधा:
        पक्ष Formatted_मानम्_विधा:
            प्रत्यागम "f-string expression";
        पक्ष Template_Str_विधा:
        पक्ष Interpolation_विधा:
            प्रत्यागम "t-string expression";
        पक्ष Constant_विधा: {
            PyObject *मानम् = e->v.Constant.मानम्;
            यदि (मानम् == Py_None) {
                प्रत्यागम "None";
            }
            यदि (मानम् == Py_False) {
                प्रत्यागम "False";
            }
            यदि (मानम् == Py_True) {
                प्रत्यागम "True";
            }
            यदि (मानम् == Py_Ellipsis) {
                प्रत्यागम "ellipsis";
            }
            प्रत्यागम "literal";
        }
        पक्ष Compare_विधा:
            प्रत्यागम "comparison";
        पक्ष If_Exp_विधा:
            प्रत्यागम "conditional expression";
        पक्ष Named_व्यञ्जकम्_विधा:
            प्रत्यागम "named expression";
        स्वभाव:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->विधा, e->lineno);
            प्रत्यागम रिक्त;
    }
}

व्यञ्जकम्_ty
Py_Pegen_get_चरमः_comprehension_item(comprehension_ty comprehension) {
    यदि (comprehension->ifs == रिक्त || asdl_seq_LEN(comprehension->ifs) == 0) {
        प्रत्यागम comprehension->iter;
    }
    प्रत्यागम Py_Pegen_चरमः_item(comprehension->ifs, व्यञ्जकम्_ty);
}

व्यञ्जकम्_ty _PyPegen_collect_call_seqs(Parser *p, asdl_व्यञ्जकम्_seq *a, asdl_seq *b,
                     पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                     पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena) {
    Py_ssize_t तर्काः_len = asdl_seq_LEN(a);
    Py_ssize_t योग_len = तर्काः_len;

    यदि (b == रिक्त) {
        प्रत्यागम _PyAST_Call(_PyPegen_dummy_name(p), a, रिक्त, lineno, col_विस्थापनम्,
                        अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);

    }

    asdl_व्यञ्जकम्_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    यदि (!starreds && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    यदि (!keywords && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }

    यदि (starreds) {
        योग_len += asdl_seq_LEN(starreds);
    }

    asdl_व्यञ्जकम्_seq *तर्काः = Py_asdl_व्यञ्जकम्_seq_new(योग_len, arena);
    यदि (तर्काः == रिक्त) {
        प्रत्यागम रिक्त;
    }

    Py_ssize_t i = 0;
    प्रति (i = 0; i < तर्काः_len; i++) {
        asdl_seq_समुच्चयः(तर्काः, i, asdl_seq_GET(a, i));
    }
    प्रति (; i < योग_len; i++) {
        asdl_seq_समुच्चयः(तर्काः, i, asdl_seq_GET(starreds, i - तर्काः_len));
    }

    प्रत्यागम _PyAST_Call(_PyPegen_dummy_name(p), तर्काः, keywords, lineno,
                       col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
}

// AST Error reporting helpers

व्यञ्जकम्_ty
_PyPegen_get_invalid_target(व्यञ्जकम्_ty e, TARGETS_TYPE targets_type)
{
    यदि (e == रिक्त) {
        प्रत्यागम रिक्त;
    }

    यदि (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        यदि (targets_type == SINGLE_TARGETS && e->विधा == Name_विधा) {
            प्रत्यागम रिक्त;
        }
        प्रत्यागम e->विधा == Attribute_विधा || e->विधा == Subscript_विधा ? रिक्त : e;
    }

#परिभाषय दर्शय_CONTAINER(CONTAINER, TYPE) कुरु { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        प्रति (Py_ssize_t i = 0; i < len; i++) {\
            व्यञ्जकम्_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            व्यञ्जकम्_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            यदि (child != रिक्त) {\
                प्रत्यागम child;\
            }\
        }\
    } यावत् (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    विकल्प (e->विधा) {
        पक्ष List_विधा:
            दर्शय_CONTAINER(e, List);
            प्रत्यागम रिक्त;
        पक्ष Tuple_विधा:
            दर्शय_CONTAINER(e, Tuple);
            प्रत्यागम रिक्त;
        पक्ष Starred_विधा:
            यदि (targets_type == DEL_TARGETS) {
                प्रत्यागम e;
            }
            प्रत्यागम _PyPegen_get_invalid_target(e->v.Starred.मानम्, targets_type);
        पक्ष Compare_विधा:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            यदि (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                यदि (cmpop == अन्तर्) {
                    प्रत्यागम _PyPegen_get_invalid_target(e->v.Compare.वामः, targets_type);
                }
                प्रत्यागम रिक्त;
            }
            प्रत्यागम e;
        पक्ष Name_विधा:
        पक्ष Subscript_विधा:
        पक्ष Attribute_विधा:
            प्रत्यागम रिक्त;
        स्वभाव:
            प्रत्यागम e;
    }
}

शून्य *_PyPegen_arguments_parsing_error(Parser *p, व्यञ्जकम्_ty e) {
    पूर्णाङ्क kwarg_unpacking = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        यदि (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ध्रुव वर्ण *सन्देशः = रिक्त;
    यदि (kwarg_unpacking) {
        सन्देशः = "positional argument follows keyword argument unpacking";
    } अन्यथा {
        सन्देशः = "positional argument follows keyword argument";
    }

    प्रत्यागम RAISE_SYNTAX_ERROR(सन्देशः);
}

शून्य *
Py_Pegen_nonparen_genexp_अन्तर्_call(Parser *p, व्यञ्जकम्_ty तर्काः, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(तर्काः->v.Call.तर्काः);
    यदि (len <= 1) {
        प्रत्यागम रिक्त;
    }

    comprehension_ty चरमः_comprehension = Py_Pegen_चरमः_item(comprehensions, comprehension_ty);

    प्रत्यागम RAISE_SYNTAX_ERROR_ज्ञातम्_RANGE(
        (व्यञ्जकम्_ty) asdl_seq_GET(तर्काः->v.Call.तर्काः, len - 1),
        Py_Pegen_get_चरमः_comprehension_item(चरमः_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

स्थिर व्यञ्जकम्_ty
_PyPegen_decode_fstring_part(Parser* p, पूर्णाङ्क is_raw, व्यञ्जकम्_ty constant, Token* token) {
    प्रतिज्ञा(PyUnicode_CheckExact(constant->v.Constant.मानम्));

    ध्रुव वर्ण* bstr = PyUnicode_AsUTF8(constant->v.Constant.मानम्);
    यदि (bstr == रिक्त) {
        प्रत्यागम रिक्त;
    }

    आकार_t len;
    यदि (वाक्यं_तुलय(bstr, "{{") == 0 || वाक्यं_तुलय(bstr, "}}") == 0) {
        len = 1;
    } अन्यथा {
        len = वाक्यदैर्घ्यम्(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == रिक्त;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    यदि (str == रिक्त) {
        _Pypegen_raise_decode_error(p);
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, constant->lineno, constant->col_विस्थापनम्,
                           constant->अन्तः_lineno, constant->अन्तः_col_विस्थापनम्,
                           p->arena);
}

स्थिर asdl_व्यञ्जकम्_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions,
                   Token *b, पूर्णाङ्क is_tstring)
{
    Py_ssize_t n_वस्तूनि = asdl_seq_LEN(raw_expressions);
    Py_ssize_t योग_वस्तूनि = n_वस्तूनि;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(raw_expressions, i);
        यदि (item->विधा == Joined_Str_विधा) {
            योग_वस्तूनि += asdl_seq_LEN(item->v.JoinedStr.मानानि) - 1;
        }
    }

    ध्रुव वर्ण* quote_str = Py_अष्टकानि_As_String(a->अष्टकानि);
    यदि (quote_str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    पूर्णाङ्क is_raw = strpbrk(quote_str, "rR") != रिक्त;

    asdl_व्यञ्जकम्_seq *seq = Py_asdl_व्यञ्जकम्_seq_new(योग_वस्तूनि, p->arena);
    यदि (seq == रिक्त) {
        प्रत्यागम रिक्त;
    }

    Py_ssize_t सूचकांक = 0;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        यदि (item->विधा == Joined_Str_विधा) {
            asdl_व्यञ्जकम्_seq *मानानि = item->v.JoinedStr.मानानि;
            यदि (asdl_seq_LEN(मानानि) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                प्रत्यागम रिक्त;
            }

            व्यञ्जकम्_ty first = asdl_seq_GET(मानानि, 0);
            प्रतिज्ञा(first->विधा == Constant_विधा);
            asdl_seq_समुच्चयः(seq, सूचकांक++, first);

            व्यञ्जकम्_ty second = asdl_seq_GET(मानानि, 1);
            प्रतिज्ञा((is_tstring &&
                    second->विधा == Interpolation_विधा) ||
                   second->विधा == Formatted_मानम्_विधा);
            asdl_seq_समुच्चयः(seq, सूचकांक++, second);

            अनुवर्त;
        }

        यदि (item->विधा == Constant_विधा) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            यदि (item == रिक्त) {
                प्रत्यागम रिक्त;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            यदि (PyUnicode_CheckExact(item->v.Constant.मानम्)
                && Py_Unicode_GET_दीर्घता(item->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
        }
        asdl_seq_समुच्चयः(seq, सूचकांक++, item);
    }

    asdl_व्यञ्जकम्_seq *resized_exprs;
    यदि (सूचकांक != योग_वस्तूनि) {
        resized_exprs = Py_asdl_व्यञ्जकम्_seq_new(सूचकांक, p->arena);
        यदि (resized_exprs == रिक्त) {
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < सूचकांक; i++) {
            asdl_seq_समुच्चयः(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    अन्यथा {
        resized_exprs = seq;
    }
    प्रत्यागम resized_exprs;
}

व्यञ्जकम्_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b) {

    asdl_व्यञ्जकम्_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    यदि (resized_exprs == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_विस्थापनम्,
                              b->अन्तः_lineno, b->अन्तः_col_विस्थापनम्,
                              p->arena);
}

व्यञ्जकम्_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_व्यञ्जकम्_seq* raw_expressions, Token*b) {

    asdl_व्यञ्जकम्_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    यदि (resized_exprs == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_विस्थापनम्,
                            b->अन्तः_lineno, b->अन्तः_col_विस्थापनम्,
                            p->arena);
}

व्यञ्जकम्_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    यदि (tok->अष्टकानि == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    Py_ssize_t bsize = Py_अष्टकानि_GET_आकार(tok->अष्टकानि);
    ध्रुव वर्ण *bstr = Py_अष्टकानि_AS_STRING(tok->अष्टकानि);

    पूर्णाङ्क is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    यदि (str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_विस्थापनम्,
                           tok->अन्तः_lineno, tok->अन्तः_col_विस्थापनम्,
                           p->arena);
}

व्यञ्जकम्_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    यदि (tok->अष्टकानि == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    ध्रुव वर्ण *bstr = Py_अष्टकानि_AS_STRING(tok->अष्टकानि);
    PyObject* str = PyUnicode_FromString(bstr);
    यदि (str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_विस्थापनम्,
                           tok->अन्तः_lineno, tok->अन्तः_col_विस्थापनम्,
                           p->arena);
}

व्यञ्जकम्_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    यदि (tok->अष्टकानि == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    ध्रुव वर्ण *the_str = Py_अष्टकानि_AS_STRING(tok->अष्टकानि);
    PyObject *s = _PyPegen_parse_string(p, tok);
    यदि (s == रिक्त) {
        _Pypegen_raise_decode_error(p);
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, s) < 0) {
        Py_DECREF(s);
        प्रत्यागम रिक्त;
    }
    PyObject *विधा = रिक्त;
    यदि (the_str && the_str[0] == 'u') {
        विधा = _PyPegen_new_identifier(p, "u");
        यदि (विधा == रिक्त) {
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम _PyAST_Constant(s, विधा, tok->lineno, tok->col_विस्थापनम्, tok->अन्तः_lineno, tok->अन्तः_col_विस्थापनम्, p->arena);
}

स्थिर पूर्णाङ्क
_get_interpolation_conversion(Parser *p, Token *debug, परिणाम_Token_With_Metadata *conversion,
                              परिणाम_Token_With_Metadata *format)
{
    यदि (conversion != रिक्त) {
        व्यञ्जकम्_ty conversion_व्यञ्जकम् = (व्यञ्जकम्_ty) conversion->परिणाम;
        प्रतिज्ञा(conversion_व्यञ्जकम्->विधा == Name_विधा);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_व्यञ्जकम्->v.Name.id, 0);
        प्रत्यागम Py_SAFE_DOWNCAST(first, Py_UCS4, पूर्णाङ्क);
    }
    अन्यथा यदि (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        प्रत्यागम (पूर्णाङ्क)'r';
    }
    प्रत्यागम -1;
}

स्थिर PyObject *
strip_interpolation_debug_व्यञ्जकम्(PyObject *exprstr)
{
    Py_ssize_t len = Py_Unicode_GET_दीर्घता(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    यावत् (len > 0) {
        पूर्णाङ्क has_newline = 0;
        यावत् (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            यदि (!_PyUnicode_IsWhitespace(c)) {
                भङ्ग;
            }
            यदि (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        यदि (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            भङ्ग;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    यदि (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        प्रत्यागम Py_NewRef(exprstr);
    }

    प्रत्यागम PyUnicode_Substring(exprstr, 0, len - 1);
}

व्यञ्जकम्_ty _PyPegen_interpolation(Parser *p, व्यञ्जकम्_ty expression, Token *debug, परिणाम_Token_With_Metadata *conversion,
                                 परिणाम_Token_With_Metadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्,
                                 पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena) {

    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_अन्तः_पङ्क्तिः, debug_अन्तः_विस्थापनम्;
    PyObject *debug_metadata;
    constant exprstr;

    यदि (conversion) {
        debug_अन्तः_पङ्क्तिः = ((व्यञ्जकम्_ty) conversion->परिणाम)->lineno;
        debug_अन्तः_विस्थापनम् = ((व्यञ्जकम्_ty) conversion->परिणाम)->col_विस्थापनम्;
        debug_metadata = exprstr = conversion->metadata;
    }
    अन्यथा यदि (format) {
        debug_अन्तः_पङ्क्तिः = ((व्यञ्जकम्_ty) format->परिणाम)->lineno;
        debug_अन्तः_विस्थापनम् = ((व्यञ्जकम्_ty) format->परिणाम)->col_विस्थापनम् + 1;
        debug_metadata = exprstr = format->metadata;
    }
    अन्यथा {
        debug_अन्तः_पङ्क्तिः = अन्तः_lineno;
        debug_अन्तः_विस्थापनम् = अन्तः_col_विस्थापनम्;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    प्रतिज्ञा(exprstr != रिक्त);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_व्यञ्जकम्(exprstr)
        : Py_NewRef(exprstr);
    यदि (!final_exprstr || Py_Arena_सङ्कलय_Py_Object(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        प्रत्यागम रिक्त;
    }

    व्यञ्जकम्_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (व्यञ्जकम्_ty) format->परिणाम : रिक्त,
        lineno, col_विस्थापनम्, अन्तः_lineno,
        अन्तः_col_विस्थापनम्, arena
    );

    यदि (!interpolation || !debug) {
        प्रत्यागम interpolation;
    }

    व्यञ्जकम्_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_विस्थापनम् + 1, debug_अन्तः_पङ्क्तिः,
                                            debug_अन्तः_विस्थापनम् - 1, p->arena);
    यदि (!debug_text) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *मानानि = Py_asdl_व्यञ्जकम्_seq_new(2, arena);
    यदि (!मानानि) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_समुच्चयः(मानानि, 0, debug_text);
    asdl_seq_समुच्चयः(मानानि, 1, interpolation);
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_विस्थापनम्, debug_अन्तः_पङ्क्तिः, debug_अन्तः_विस्थापनम्, p->arena);
}

व्यञ्जकम्_ty Py_Pegen_formatted_मानम्(Parser *p, व्यञ्जकम्_ty expression, Token *debug, परिणाम_Token_With_Metadata *conversion,
                                 परिणाम_Token_With_Metadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्,
                                 पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena) {
    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    व्यञ्जकम्_ty formatted_मानम् = Py_AST_Formatted_मानम्(
        expression, conversion_val, format ? (व्यञ्जकम्_ty) format->परिणाम : रिक्त,
        lineno, col_विस्थापनम्, अन्तः_lineno,
        अन्तः_col_विस्थापनम्, arena
    );

    यदि (!formatted_मानम् || !debug) {
        प्रत्यागम formatted_मानम्;
    }

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_अन्तः_पङ्क्तिः, debug_अन्तः_विस्थापनम्;
    PyObject *debug_metadata;

    यदि (conversion) {
        debug_अन्तः_पङ्क्तिः = ((व्यञ्जकम्_ty) conversion->परिणाम)->lineno;
        debug_अन्तः_विस्थापनम् = ((व्यञ्जकम्_ty) conversion->परिणाम)->col_विस्थापनम्;
        debug_metadata = conversion->metadata;
    }
    अन्यथा यदि (format) {
        debug_अन्तः_पङ्क्तिः = ((व्यञ्जकम्_ty) format->परिणाम)->lineno;
        debug_अन्तः_विस्थापनम् = ((व्यञ्जकम्_ty) format->परिणाम)->col_विस्थापनम् + 1;
        debug_metadata = format->metadata;
    }
    अन्यथा {
        debug_अन्तः_पङ्क्तिः = अन्तः_lineno;
        debug_अन्तः_विस्थापनम् = अन्तः_col_विस्थापनम्;
        debug_metadata = closing_brace->metadata;
    }
    व्यञ्जकम्_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_विस्थापनम् + 1, debug_अन्तः_पङ्क्तिः,
                                            debug_अन्तः_विस्थापनम् - 1, p->arena);
    यदि (!debug_text) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *मानानि = Py_asdl_व्यञ्जकम्_seq_new(2, arena);
    यदि (!मानानि) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_समुच्चयः(मानानि, 0, debug_text);
    asdl_seq_समुच्चयः(मानानि, 1, formatted_मानम्);
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_विस्थापनम्, debug_अन्तः_पङ्क्तिः, debug_अन्तः_विस्थापनम्, p->arena);
}

स्थिर व्यञ्जकम्_ty
build_concatenated_अष्टकानि(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि, पूर्णाङ्क lineno,
                        पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(वाक्यानि);
    प्रतिज्ञा(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *विधा = asdl_seq_GET(वाक्यानि, 0)->v.Constant.विधा;

    Py_ssize_t योग = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(वाक्यानि, i);
        PyObject *अष्टकानि = elem->v.Constant.मानम्;
        Py_ssize_t part = Py_अष्टकानि_GET_आकार(अष्टकानि);
        यदि (part > PY_SSIZE_T_MAX - योग) {
            Py_Err_No_स्मृतिः();
            प्रत्यागम रिक्त;
        }
        योग += part;
    }

    Py_अष्टकानि_Writer *writer = Py_अष्टकानि_Writer_Create(योग);
    यदि (writer == रिक्त) {
        प्रत्यागम रिक्त;
    }
    वर्ण *बहिर्गमनम् = Py_अष्टकानि_Writer_Get_दत्त(writer);

    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(वाक्यानि, i);
        PyObject *अष्टकानि = elem->v.Constant.मानम्;
        Py_ssize_t part = Py_अष्टकानि_GET_आकार(अष्टकानि);
        यदि (part > 0) {
            स्मृतिं_प्रतिलिख(बहिर्गमनम्, Py_अष्टकानि_AS_STRING(अष्टकानि), part);
            बहिर्गमनम् += part;
        }
    }

    PyObject *res = Py_अष्टकानि_Writer_Finish(writer);
    यदि (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(arena, res) < 0) {
        Py_DECREF(res);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(res, विधा, lineno, col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, p->arena);
}

स्थिर व्यञ्जकम्_ty
_build_concatenated_unicode(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि, पूर्णाङ्क lineno,
                        पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(वाक्यानि);
    प्रतिज्ञा(len > 1);

    व्यञ्जकम्_ty first = asdl_seq_GET(वाक्यानि, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *विधा = first->v.Constant.विधा;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    यदि (writer == रिक्त) {
        प्रत्यागम रिक्त;
    }

    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty current_elem = asdl_seq_GET(वाक्यानि, i);
        प्रतिज्ञा(current_elem->विधा == Constant_विधा);

        यदि (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.मानम्)) {
            PyUnicodeWriter_Discard(writer);
            प्रत्यागम रिक्त;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    यदि (final == रिक्त) {
        प्रत्यागम रिक्त;
    }
    यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, final) < 0) {
        Py_DECREF(final);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(final, विधा, lineno, col_विस्थापनम्,
                           अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
}

स्थिर asdl_व्यञ्जकम्_seq *
_build_concatenated_str(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                               पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(वाक्यानि);
    प्रतिज्ञा(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(वाक्यानि, i);
        विकल्प(elem->विधा) {
            पक्ष Joined_Str_विधा:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.मानानि);
                भङ्ग;
            पक्ष Template_Str_विधा:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.मानानि);
                भङ्ग;
            स्वभाव:
                n_flattened_elements++;
                भङ्ग;
        }
    }


    asdl_व्यञ्जकम्_seq* flattened = Py_asdl_व्यञ्जकम्_seq_new(n_flattened_elements, p->arena);
    यदि (flattened == रिक्त) {
        प्रत्यागम रिक्त;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(वाक्यानि, i);
        विकल्प(elem->विधा) {
            पक्ष Joined_Str_विधा:
                प्रति (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.मानानि); j++) {
                    व्यञ्जकम्_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.मानानि, j);
                    यदि (subvalue == रिक्त) {
                        प्रत्यागम रिक्त;
                    }
                    asdl_seq_समुच्चयः(flattened, current_pos++, subvalue);
                }
                भङ्ग;
            पक्ष Template_Str_विधा:
                प्रति (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.मानानि); j++) {
                    व्यञ्जकम्_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.मानानि, j);
                    यदि (subvalue == रिक्त) {
                        प्रत्यागम रिक्त;
                    }
                    asdl_seq_समुच्चयः(flattened, current_pos++, subvalue);
                }
                भङ्ग;
            स्वभाव:
                asdl_seq_समुच्चयः(flattened, current_pos++, elem);
                भङ्ग;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    पूर्णाङ्क पूर्व_is_constant = 0;
    प्रति (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        यदि (elem->विधा == Constant_विधा &&
            PyUnicode_CheckExact(elem->v.Constant.मानम्) &&
            Py_Unicode_GET_दीर्घता(elem->v.Constant.मानम्) == 0)
            अनुवर्त;

        यदि (!पूर्व_is_constant || elem->विधा != Constant_विधा) {
            n_elements++;
        }
        पूर्व_is_constant = elem->विधा == Constant_विधा;
    }

    asdl_व्यञ्जकम्_seq* मानानि = Py_asdl_व्यञ्जकम्_seq_new(n_elements, p->arena);
    यदि (मानानि == रिक्त) {
        प्रत्यागम रिक्त;
    }

    /* build folded list */
    current_pos = 0;
    प्रति (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        यदि (elem->विधा == Constant_विधा) {
            यदि (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->विधा == Constant_विधा) {
                व्यञ्जकम्_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *विधा = elem->v.Constant.विधा;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                यदि (writer == रिक्त) {
                    प्रत्यागम रिक्त;
                }
                व्यञ्जकम्_ty चरमः_elem = elem;
                Py_ssize_t j;
                प्रति (j = i; j < n_flattened_elements; j++) {
                    व्यञ्जकम्_ty current_elem = asdl_seq_GET(flattened, j);
                    यदि (current_elem->विधा == Constant_विधा) {
                        यदि (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.मानम्)) {
                            PyUnicodeWriter_Discard(writer);
                            प्रत्यागम रिक्त;
                        }
                        चरमः_elem = current_elem;
                    } अन्यथा {
                        भङ्ग;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                यदि (concat_str == रिक्त) {
                    प्रत्यागम रिक्त;
                }
                यदि (Py_Arena_सङ्कलय_Py_Object(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    प्रत्यागम रिक्त;
                }
                elem = _PyAST_Constant(concat_str, विधा, first_elem->lineno,
                                       first_elem->col_विस्थापनम्,
                                       चरमः_elem->अन्तः_lineno,
                                       चरमः_elem->अन्तः_col_विस्थापनम्, p->arena);
                यदि (elem == रिक्त) {
                    प्रत्यागम रिक्त;
                }
            }

            /* Drop all empty contanst strings */
            यदि (PyUnicode_CheckExact(elem->v.Constant.मानम्) &&
                Py_Unicode_GET_दीर्घता(elem->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
        }

        asdl_seq_समुच्चयः(मानानि, current_pos++, elem);
    }

    प्रतिज्ञा(current_pos == n_elements);
    प्रत्यागम मानानि;
}

स्थिर व्यञ्जकम्_ty
_build_concatenated_joined_str(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                               पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena)
{
    asdl_व्यञ्जकम्_seq *मानानि = _build_concatenated_str(p, वाक्यानि, lineno,
        col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
    यदि (!मानानि) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, p->arena);
}

व्यञ्जकम्_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                               पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena)
{
    asdl_व्यञ्जकम्_seq *मानानि = _build_concatenated_str(p, वाक्यानि, lineno,
        col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
    यदि (!मानानि) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_TemplateStr(मानानि, lineno, col_विस्थापनम्, अन्तः_lineno,
        अन्तः_col_विस्थापनम्, arena);
}

व्यञ्जकम्_ty
Py_Pegen_concatenate_वाक्यानि(Parser *p, asdl_व्यञ्जकम्_seq *वाक्यानि,
                             पूर्णाङ्क lineno, पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno,
                             पूर्णाङ्क अन्तः_col_विस्थापनम्, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(वाक्यानि);
    प्रतिज्ञा(len > 0);

    पूर्णाङ्क f_string_प्राप्तम् = 0;
    पूर्णाङ्क unicode_string_प्राप्तम् = 0;
    पूर्णाङ्क अष्टकानि_प्राप्तम् = 0;

    Py_ssize_t i = 0;
    प्रति (i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(वाक्यानि, i);
        विकल्प(elem->विधा) {
            पक्ष Constant_विधा:
                यदि (Py_अष्टकानि_Check_Exact(elem->v.Constant.मानम्)) {
                    अष्टकानि_प्राप्तम् = 1;
                } अन्यथा {
                    unicode_string_प्राप्तम् = 1;
                }
                भङ्ग;
            पक्ष Joined_Str_विधा:
                f_string_प्राप्तम् = 1;
                भङ्ग;
            पक्ष Template_Str_विधा:
                // python.gram handles this; we should never get here
                प्रतिज्ञा(0);
                भङ्ग;
            स्वभाव:
                f_string_प्राप्तम् = 1;
                भङ्ग;
        }
    }

    // Cannot mix unicode and bytes
    यदि ((unicode_string_प्राप्तम् || f_string_प्राप्तम्) && अष्टकानि_प्राप्तम्) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        प्रत्यागम रिक्त;
    }

    // If it's only bytes or only unicode string, do a simple concat
    यदि (!f_string_प्राप्तम्) {
        यदि (len == 1) {
            प्रत्यागम asdl_seq_GET(वाक्यानि, 0);
        }
        अन्यथा यदि (अष्टकानि_प्राप्तम्) {
            प्रत्यागम build_concatenated_अष्टकानि(p, वाक्यानि, lineno, col_विस्थापनम्,
                अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
        }
        अन्यथा {
            प्रत्यागम _build_concatenated_unicode(p, वाक्यानि, lineno, col_विस्थापनम्,
                अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
        }
    }

    प्रत्यागम _build_concatenated_joined_str(p, वाक्यानि, lineno,
        col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
}

स्थिर पूर्णाङ्क
warn_relative_import_सम्बन्धि_lazy(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    यदि (p->call_invalid_नियमाः) {
        प्रत्यागम 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *चरमः_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    यदि (
        चरमः_dot->अन्तः_lineno == module->lineno
        && चरमः_dot->अन्तः_col_विस्थापनम् == module->col_विस्थापनम्
    ) {
        प्रत्यागम 0;
    }

    पूर्णाङ्क गणनम् = Py_Pegen_seq_गणनम्_dots(dots);
    वर्ण *buf = PyMem_RawMalloc(गणनम् + 1);
    यदि (buf == रिक्त) {
        Py_Err_No_स्मृतिः();
        प्रत्यागम -1;
    }
    स्मृतिं_पूरय(buf, '.', गणनम्);
    buf[गणनम्] = '\0';

    PyObject *सन्देशः = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    यदि (सन्देशः == रिक्त) {
        प्रत्यागम -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    पूर्णाङ्क res = _PyErr_EmitSyntaxWarning(सन्देशः,
                                       info.filename,
                                       module->lineno,
                                       module->col_विस्थापनम् + 1,
                                       module->अन्तः_lineno,
                                       module->अन्तः_col_विस्थापनम् + 1,
                                       info.module);
    Py_DECREF(सन्देशः);
    प्रत्यागम res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module_name,
                             asdl_alias_seq *नामानि, व्यञ्जकम्_ty lazy_token, पूर्णाङ्क lineno,
                             पूर्णाङ्क col_विस्थापनम्, पूर्णाङ्क अन्तः_lineno, पूर्णाङ्क अन्तः_col_विस्थापनम्,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    पूर्णाङ्क level = Py_Pegen_seq_गणनम्_dots(dots);
    यदि (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        यदि (lazy_token) {
            RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < asdl_seq_LEN(नामानि); i++) {
            alias_ty alias = asdl_seq_GET(नामानि, i);
            यदि (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    अन्यथा यदि (
        level > 0
        && lazy_token == रिक्त
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        यदि (warn_relative_import_सम्बन्धि_lazy(p, dots, module_name) < 0) {
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम _PyAST_ImportFrom(module, नामानि, level, lazy_token ? 1 : 0, lineno,
                             col_विस्थापनम्, अन्तः_lineno, अन्तः_col_विस्थापनम्, arena);
}

asdl_stmt_seq*
Py_Pegen_पञ्जिकाकोशः_stmts(Parser *p, asdl_stmt_seq* stmts) {
    यदि (!p->call_invalid_नियमाः) {
        प्रत्यागम stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    यदि (len == 0) {
        प्रत्यागम stmts;
    }
    stmt_ty चरमः_stmt = asdl_seq_GET(stmts, len - 1);
    यदि (p->चरमः_stmt_location.lineno > चरमः_stmt->lineno) {
        प्रत्यागम stmts;
    }
    p->चरमः_stmt_location.lineno = चरमः_stmt->lineno;
    p->चरमः_stmt_location.col_विस्थापनम् = चरमः_stmt->col_विस्थापनम्;
    p->चरमः_stmt_location.अन्तः_lineno = चरमः_stmt->अन्तः_lineno;
    p->चरमः_stmt_location.अन्तः_col_विस्थापनम् = चरमः_stmt->अन्तः_col_विस्थापनम्;
    प्रत्यागम stmts;
}
