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
    जइ (!seq) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    प्रत्यागम seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_निवेशय_in_front(Parser *p, शून्य *a, asdl_seq *seq)
{
    प्रतिज्ञा(a != रिक्त);
    जइ (!seq) {
        प्रत्यागम _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    प्रति (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    प्रत्यागम new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    जइ (!seq) {
        प्रत्यागम _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }

    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    प्रत्यागम new_seq;
}

स्थिर Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    प्रत्यागम size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    प्रतिज्ञा(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    जइ (!flattened_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क flattened_seq_idx = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        प्रति (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    प्रतिज्ञा(flattened_seq_idx == flattened_seq_size);

    प्रत्यागम flattened_seq;
}

शून्य *
_PyPegen_seq_last_item(asdl_seq *seq)
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
_PyPegen_join_names_with_dot(Parser *p, व्यञ्जकम्_ty first_name, व्यञ्जकम्_ty second_name)
{
    प्रतिज्ञा(first_name != रिक्त && second_name != रिक्त);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    जइ (!uni) {
        प्रत्यागम रिक्त;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    जइ (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        प्रत्यागम रिक्त;
    }

    प्रत्यागम _PyAST_Name(uni, Load, EXTRA_व्यञ्जकम्(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
पूर्णाङ्क
Py_Pegen_seq_गणनम्_dots(asdl_seq *seq)
{
    पूर्णाङ्क सङ्ख्या_of_dots = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_व्यञ्जकम् = asdl_seq_GET_UNTYPED(seq, i);
        विकल्प (current_व्यञ्जकम्->type) {
            पक्ष ELLIPSIS:
                सङ्ख्या_of_dots += 3;
                भङ्ग;
            पक्ष DOT:
                सङ्ख्या_of_dots += 1;
                भङ्ग;
            स्वभाव:
                Py_UNREACHABLE();
        }
    }

    प्रत्यागम सङ्ख्या_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                        पूर्णाङ्क end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    जइ (!str) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_alias(str, रिक्त, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_व्यञ्जकम्_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    प्रत्यागम new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_व्यञ्जकम्_Pair *
Py_Pegen_cmpop_व्यञ्जकम्_pair(Parser *p, cmpop_ty cmpop, व्यञ्जकम्_ty व्यञ्जकम्)
{
    प्रतिज्ञा(व्यञ्जकम् != रिक्त);
    Cmpop_व्यञ्जकम्_Pair *a = _PyArena_Malloc(p->arena, परिमाण(Cmpop_व्यञ्जकम्_Pair));
    जइ (!a) {
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
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_व्यञ्जकम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    प्रत्यागम new_seq;
}

asdl_व्यञ्जकम्_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_व्यञ्जकम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->व्यञ्जकम्);
    }
    प्रत्यागम new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
स्थिर asdl_व्यञ्जकम्_seq *
_set_seq_context(Parser *p, asdl_व्यञ्जकम्_seq *seq, व्यञ्जकम्_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    जइ (len == 0) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty e = asdl_seq_GET(seq, i);
        व्यञ्जकम्_ty new_e = Py_Pegen_set_व्यञ्जकम्_context(p, e, ctx);
        जइ (!new_e) {
            प्रत्यागम रिक्त;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    प्रत्यागम new_seq;
}

स्थिर व्यञ्जकम्_ty
_set_name_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Name(e->v.Name.id, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
_set_tuple_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    asdl_व्यञ्जकम्_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    जइ (!seq && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Tuple(seq, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
_set_list_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    asdl_व्यञ्जकम्_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    जइ (!seq && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_List(seq, ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
_set_subscript_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Subscript(e->v.Subscript.मानम्, e->v.Subscript.slice,
                            ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
_set_attribute_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    प्रत्यागम _PyAST_Attribute(e->v.Attribute.मानम्, e->v.Attribute.attr,
                            ctx, EXTRA_व्यञ्जकम्(e, e));
}

स्थिर व्यञ्जकम्_ty
_set_starred_context(Parser *p, व्यञ्जकम्_ty e, व्यञ्जकम्_context_ty ctx)
{
    व्यञ्जकम्_ty inner = Py_Pegen_set_व्यञ्जकम्_context(p, e->v.Starred.मानम्, ctx);
    जइ (!inner) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Starred(inner, ctx, EXTRA_व्यञ्जकम्(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
व्यञ्जकम्_ty
Py_Pegen_set_व्यञ्जकम्_context(Parser *p, व्यञ्जकम्_ty व्यञ्जकम्, व्यञ्जकम्_context_ty ctx)
{
    प्रतिज्ञा(व्यञ्जकम् != रिक्त);

    व्यञ्जकम्_ty new = रिक्त;
    विकल्प (व्यञ्जकम्->kind) {
        पक्ष Name_kind:
            new = _set_name_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Tuple_kind:
            new = _set_tuple_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष List_kind:
            new = _set_list_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Subscript_kind:
            new = _set_subscript_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Attribute_kind:
            new = _set_attribute_context(p, व्यञ्जकम्, ctx);
            भङ्ग;
        पक्ष Starred_kind:
            new = _set_starred_context(p, व्यञ्जकम्, ctx);
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
    जइ (_PyPegen_get_invalid_target(व्यञ्जकम्, targets_type) != रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम Py_Pegen_set_व्यञ्जकम्_context(
        p, व्यञ्जकम्, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
कुञ्चिका_मानम्_Pair *
Py_Pegen_कुञ्चिका_मानम्_pair(Parser *p, व्यञ्जकम्_ty कुञ्चिका, व्यञ्जकम्_ty मानम्)
{
    कुञ्चिका_मानम्_Pair *a = _PyArena_Malloc(p->arena, परिमाण(कुञ्चिका_मानम्_Pair));
    जइ (!a) {
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
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_मानम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->कुञ्चिका);
    }
    प्रत्यागम new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_व्यञ्जकम्_seq *
Py_Pegen_get_मानानि(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_मानम्_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->मानम्);
    }
    प्रत्यागम new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
कुञ्चिका_Pattern_Pair *
Py_Pegen_कुञ्चिका_pattern_pair(Parser *p, व्यञ्जकम्_ty कुञ्चिका, pattern_ty pattern)
{
    कुञ्चिका_Pattern_Pair *a = _PyArena_Malloc(p->arena, परिमाण(कुञ्चिका_Pattern_Pair));
    जइ (!a) {
        प्रत्यागम रिक्त;
    }
    a->कुञ्चिका = कुञ्चिका;
    a->pattern = pattern;
    प्रत्यागम a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_व्यञ्जकम्_seq *
Py_Pegen_get_pattern_कुञ्चिकाः(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->कुञ्चिका);
    }
    प्रत्यागम new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        कुञ्चिका_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    प्रत्यागम new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, व्यञ्जकम्_ty मानम्, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, परिमाण(NameDefaultPair));
    जइ (!a) {
        प्रत्यागम रिक्त;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    जइ (!a->arg) {
        प्रत्यागम रिक्त;
    }
    a->मानम् = मानम्;
    प्रत्यागम a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, परिमाण(SlashWithDefault));
    जइ (!a) {
        प्रत्यागम रिक्त;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    प्रत्यागम a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, परिमाण(StarEtc));
    जइ (!a) {
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
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क k = 0;
    प्रति (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    प्रति (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    प्रत्यागम new_seq;
}

स्थिर asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    जइ (!seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    प्रत्यागम seq;
}

स्थिर asdl_व्यञ्जकम्_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_व्यञ्जकम्_seq *seq = Py_asdl_व्यञ्जकम्_seq_new(len, p->arena);
    जइ (!seq) {
        प्रत्यागम रिक्त;
    }
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->मानम्);
    }
    प्रत्यागम seq;
}

स्थिर पूर्णाङ्क
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    जइ (slash_without_default != रिक्त) {
        *posonlyargs = slash_without_default;
    }
    अण्णहा जइ (slash_with_default != रिक्त) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        जइ (!slash_with_default_names) {
            प्रत्यागम -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    अण्णहा {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    प्रत्यागम *posonlyargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    जइ (names_with_default != रिक्त) {
        जइ (plain_names != रिक्त) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            जइ (!names_with_default_names) {
                प्रत्यागम -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        अण्णहा {
            *posargs = _get_names(p, names_with_default);
        }
    }
    अण्णहा {
        जइ (plain_names != रिक्त) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            प्रतिज्ञा(0);
            *posargs = plain_names;
        }
        अण्णहा {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    प्रत्यागम *posargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_व्यञ्जकम्_seq **posdefaults) {
    जइ (slash_with_default != रिक्त && names_with_default != रिक्त) {
        asdl_व्यञ्जकम्_seq *slash_with_default_मानानि =
                _get_defaults(p, slash_with_default->names_with_defaults);
        जइ (!slash_with_default_मानानि) {
            प्रत्यागम -1;
        }
        asdl_व्यञ्जकम्_seq *names_with_default_मानानि = _get_defaults(p, names_with_default);
        जइ (!names_with_default_मानानि) {
            प्रत्यागम -1;
        }
        *posdefaults = (asdl_व्यञ्जकम्_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_मानानि,
                (asdl_seq*)names_with_default_मानानि);
    }
    अण्णहा जइ (slash_with_default == रिक्त && names_with_default != रिक्त) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    अण्णहा जइ (slash_with_default != रिक्त && names_with_default == रिक्त) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    अण्णहा {
        *posdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    }
    प्रत्यागम *posdefaults == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_व्यञ्जकम्_seq **kwdefaults) {
    जइ (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    अण्णहा {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    जइ (*kwonlyargs == रिक्त) {
        प्रत्यागम -1;
    }

    जइ (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    अण्णहा {
        *kwdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    }

    जइ (*kwdefaults == रिक्त) {
        प्रत्यागम -1;
    }

    प्रत्यागम 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    जइ (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        प्रत्यागम रिक्त;
    }

    asdl_arg_seq *posargs;
    जइ (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *posdefaults;
    जइ (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        प्रत्यागम रिक्त;
    }

    arg_ty vararg = रिक्त;
    जइ (star_etc != रिक्त && star_etc->vararg != रिक्त) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_व्यञ्जकम्_seq *kwdefaults;
    जइ (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        प्रत्यागम रिक्त;
    }

    arg_ty kwarg = रिक्त;
    जइ (star_etc != रिक्त && star_etc->kwarg != रिक्त) {
        kwarg = star_etc->kwarg;
    }

    प्रत्यागम _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    जइ (!posonlyargs) {
        प्रत्यागम रिक्त;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    जइ (!posargs) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *posdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    जइ (!posdefaults) {
        प्रत्यागम रिक्त;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    जइ (!kwonlyargs) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *kwdefaults = Py_asdl_व्यञ्जकम्_seq_new(0, p->arena);
    जइ (!kwdefaults) {
        प्रत्यागम रिक्त;
    }

    प्रत्यागम _PyAST_arguments(posonlyargs, posargs, रिक्त, kwonlyargs,
                            kwdefaults, रिक्त, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, परिमाण(AugOperator));
    जइ (!a) {
        प्रत्यागम रिक्त;
    }
    a->kind = kind;
    प्रत्यागम a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_व्यञ्जकम्_seq *decorators, stmt_ty function_def)
{
    प्रतिज्ञा(function_def != रिक्त);
    जइ (function_def->kind == AsyncFunctionDef_kind) {
        प्रत्यागम _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    प्रत्यागम _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_व्यञ्जकम्_seq *decorators, stmt_ty class_def)
{
    प्रतिज्ञा(class_def != रिक्त);
    प्रत्यागम _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, शून्य *element, पूर्णाङ्क is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, परिमाण(KeywordOrStarred));
    जइ (!a) {
        प्रत्यागम रिक्त;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    प्रत्यागम a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
स्थिर पूर्णाङ्क
seq_सङ्ख्या_of_starred_exprs(asdl_seq *seq)
{
    पूर्णाङ्क n = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        जइ (!k->is_keyword) {
            n++;
        }
    }
    प्रत्यागम n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_व्यञ्जकम्_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    पूर्णाङ्क new_len = seq_सङ्ख्या_of_starred_exprs(kwargs);
    जइ (new_len == 0) {
        प्रत्यागम रिक्त;
    }
    asdl_व्यञ्जकम्_seq *new_seq = Py_asdl_व्यञ्जकम्_seq_new(new_len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क idx = 0;
    प्रति (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        जइ (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    प्रत्यागम new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_सङ्ख्या_of_starred_exprs(kwargs);
    जइ (new_len == 0) {
        प्रत्यागम रिक्त;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    जइ (!new_seq) {
        प्रत्यागम रिक्त;
    }

    पूर्णाङ्क idx = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        जइ (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    प्रत्यागम new_seq;
}

व्यञ्जकम्_ty
_PyPegen_ensure_imaginary(Parser *p, व्यञ्जकम्_ty exp)
{
    जइ (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.मानम्)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        प्रत्यागम रिक्त;
    }
    प्रत्यागम exp;
}

व्यञ्जकम्_ty
_PyPegen_ensure_real(Parser *p, व्यञ्जकम्_ty exp)
{
    जइ (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.मानम्)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        प्रत्यागम रिक्त;
    }
    प्रत्यागम exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = रिक्त;
    Py_ssize_t अङ्कः = p->type_ignore_comments.अङ्कः_वस्तूनि;
    जइ (अङ्कः > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(अङ्कः, p->arena);
        जइ (type_ignores == रिक्त) {
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < अङ्कः; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.वस्तूनि[i].comment);
            जइ (tag == रिक्त) {
                प्रत्यागम रिक्त;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.वस्तूनि[i].lineno,
                                                  tag, p->arena);
            जइ (ti == रिक्त) {
                प्रत्यागम रिक्त;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    प्रत्यागम _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, ध्रुव वर्ण *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), रिक्त);
    जइ (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    जइ (tc == रिक्त) {
        प्रत्यागम a;
    }
    ध्रुव वर्ण *bytes = PyBytes_AsString(tc->bytes);
    जइ (bytes == रिक्त) {
        प्रत्यागम रिक्त;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    जइ (tco == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
पूर्णाङ्क
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    प्रतिज्ञा(t->bytes != रिक्त);
    प्रतिज्ञा(t->type == NOTEQUAL);

    ध्रुव वर्ण* tok_str = PyBytes_AS_STRING(t->bytes);
    जइ (p->flags & PyPARSE_BARRY_AS_BDFL && strcmp(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        प्रत्यागम -1;
    }
    जइ (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        प्रत्यागम strcmp(tok_str, "!=");
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyPegen_check_legacy_stmt(Parser *p, व्यञ्जकम्_ty name) {
    जइ (name->kind != Name_kind) {
        प्रत्यागम 0;
    }
    ध्रुव वर्ण* candidates[2] = {"print", "exec"};
    प्रति (पूर्णाङ्क i=0; i<2; i++) {
        जइ (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            प्रत्यागम 1;
        }
    }
    प्रत्यागम 0;
}

शून्य *
_PyPegen_raise_error_for_missing_comma(Parser *p, व्यञ्जकम्_ty a, व्यञ्जकम्_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    जइ (_PyPegen_check_legacy_stmt(p, a)) {
        प्रत्यागम रिक्त;
    }
    // Only raise inside parentheses/brackets (level > 0)
    जइ (p->tokens[p->mark - 1]->level == 0) {
        प्रत्यागम रिक्त;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    जइ (a->end_lineno > a->lineno) {
        प्रत्यागम RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    प्रत्यागम RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

स्थिर ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, शून्य *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, परिमाण(ResultTokenWithMetadata));
    जइ (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    res->metadata = metadata;
    res->result = result;
    प्रत्यागम res;
}

स्थिर वर्ण
formatted_string_prefix(ध्रुव Parser *p)
{
    पूर्णाङ्क nested = 0;
    प्रति (पूर्णाङ्क i = p->mark - 1; i >= 0; i--) {
        पूर्णाङ्क type = p->tokens[i]->type;
        जइ (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        अण्णहा जइ (type == FSTRING_START || type == TSTRING_START) {
            जइ (nested == 0) {
                प्रत्यागम type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, व्यञ्जकम्_ty conv)
{
    जइ (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        प्रत्यागम RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    जइ (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        प्रत्यागम रिक्त;
    }

    प्रत्यागम result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_व्यञ्जकम्_seq *spec, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena)
{
    जइ (!spec) {
        प्रत्यागम रिक्त;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_वस्तूनि = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_गणनम् = 0;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(spec, i);
        non_empty_गणनम् += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.मानम्) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.मानम्) == 0);
    }
    जइ (non_empty_गणनम् != n_वस्तूनि) {
        asdl_व्यञ्जकम्_seq *resized_spec =
            Py_asdl_व्यञ्जकम्_seq_new(non_empty_गणनम्, p->arena);
        जइ (resized_spec == रिक्त) {
            प्रत्यागम रिक्त;
        }
        Py_ssize_t j = 0;
        प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
            व्यञ्जकम्_ty item = asdl_seq_GET(spec, i);
            जइ (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.मानम्) &&
                PyUnicode_GET_LENGTH(item->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        प्रतिज्ञा(j == non_empty_गणनम्);
        spec = resized_spec;
    }
    व्यञ्जकम्_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    जइ (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } अण्णहा {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    जइ (!res) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम result_token_with_metadata(p, res, colon->metadata);
}

ध्रुव वर्ण *
Py_Pegen_get_व्यञ्जकम्_name(व्यञ्जकम्_ty e)
{
    प्रतिज्ञा(e != रिक्त);
    विकल्प (e->kind) {
        पक्ष Attribute_kind:
            प्रत्यागम "attribute";
        पक्ष Subscript_kind:
            प्रत्यागम "subscript";
        पक्ष Starred_kind:
            प्रत्यागम "starred";
        पक्ष Name_kind:
            प्रत्यागम "name";
        पक्ष List_kind:
            प्रत्यागम "list";
        पक्ष Tuple_kind:
            प्रत्यागम "tuple";
        पक्ष Lambda_kind:
            प्रत्यागम "lambda";
        पक्ष Call_kind:
            प्रत्यागम "function call";
        पक्ष BoolOp_kind:
        पक्ष BinOp_kind:
        पक्ष UnaryOp_kind:
            प्रत्यागम "expression";
        पक्ष GeneratorExp_kind:
            प्रत्यागम "generator expression";
        पक्ष Yield_kind:
        पक्ष YieldFrom_kind:
            प्रत्यागम "yield expression";
        पक्ष Await_kind:
            प्रत्यागम "await expression";
        पक्ष ListComp_kind:
            प्रत्यागम "list comprehension";
        पक्ष SetComp_kind:
            प्रत्यागम "set comprehension";
        पक्ष DictComp_kind:
            प्रत्यागम "dict comprehension";
        पक्ष Dict_kind:
            प्रत्यागम "dict literal";
        पक्ष Set_kind:
            प्रत्यागम "set display";
        पक्ष JoinedStr_kind:
        पक्ष Formatted_मानम्_kind:
            प्रत्यागम "f-string expression";
        पक्ष TemplateStr_kind:
        पक्ष Interpolation_kind:
            प्रत्यागम "t-string expression";
        पक्ष Constant_kind: {
            PyObject *मानम् = e->v.Constant.मानम्;
            जइ (मानम् == Py_None) {
                प्रत्यागम "None";
            }
            जइ (मानम् == Py_False) {
                प्रत्यागम "False";
            }
            जइ (मानम् == Py_True) {
                प्रत्यागम "True";
            }
            जइ (मानम् == Py_Ellipsis) {
                प्रत्यागम "ellipsis";
            }
            प्रत्यागम "literal";
        }
        पक्ष Compare_kind:
            प्रत्यागम "comparison";
        पक्ष IfExp_kind:
            प्रत्यागम "conditional expression";
        पक्ष Named_व्यञ्जकम्_kind:
            प्रत्यागम "named expression";
        स्वभाव:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            प्रत्यागम रिक्त;
    }
}

व्यञ्जकम्_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    जइ (comprehension->ifs == रिक्त || asdl_seq_LEN(comprehension->ifs) == 0) {
        प्रत्यागम comprehension->iter;
    }
    प्रत्यागम PyPegen_last_item(comprehension->ifs, व्यञ्जकम्_ty);
}

व्यञ्जकम्_ty _PyPegen_collect_call_seqs(Parser *p, asdl_व्यञ्जकम्_seq *a, asdl_seq *b,
                     पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                     पूर्णाङ्क end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    जइ (b == रिक्त) {
        प्रत्यागम _PyAST_Call(_PyPegen_dummy_name(p), a, रिक्त, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_व्यञ्जकम्_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    जइ (!starreds && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    जइ (!keywords && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }

    जइ (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_व्यञ्जकम्_seq *args = Py_asdl_व्यञ्जकम्_seq_new(total_len, arena);
    जइ (args == रिक्त) {
        प्रत्यागम रिक्त;
    }

    Py_ssize_t i = 0;
    प्रति (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    प्रति (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    प्रत्यागम _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

व्यञ्जकम्_ty
_PyPegen_get_invalid_target(व्यञ्जकम्_ty e, TARGETS_TYPE targets_type)
{
    जइ (e == रिक्त) {
        प्रत्यागम रिक्त;
    }

    जइ (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        जइ (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            प्रत्यागम रिक्त;
        }
        प्रत्यागम e->kind == Attribute_kind || e->kind == Subscript_kind ? रिक्त : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) करेहि { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        प्रति (Py_ssize_t i = 0; i < len; i++) {\
            व्यञ्जकम्_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            व्यञ्जकम्_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            जइ (child != रिक्त) {\
                प्रत्यागम child;\
            }\
        }\
    } जाव (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    विकल्प (e->kind) {
        पक्ष List_kind:
            VISIT_CONTAINER(e, List);
            प्रत्यागम रिक्त;
        पक्ष Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            प्रत्यागम रिक्त;
        पक्ष Starred_kind:
            जइ (targets_type == DEL_TARGETS) {
                प्रत्यागम e;
            }
            प्रत्यागम _PyPegen_get_invalid_target(e->v.Starred.मानम्, targets_type);
        पक्ष Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            जइ (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                जइ (cmpop == In) {
                    प्रत्यागम _PyPegen_get_invalid_target(e->v.Compare.वामः, targets_type);
                }
                प्रत्यागम रिक्त;
            }
            प्रत्यागम e;
        पक्ष Name_kind:
        पक्ष Subscript_kind:
        पक्ष Attribute_kind:
            प्रत्यागम रिक्त;
        स्वभाव:
            प्रत्यागम e;
    }
}

शून्य *_PyPegen_arguments_parsing_error(Parser *p, व्यञ्जकम्_ty e) {
    पूर्णाङ्क kwarg_unpacking = 0;
    प्रति (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        जइ (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    ध्रुव वर्ण *सन्देशः = रिक्त;
    जइ (kwarg_unpacking) {
        सन्देशः = "positional argument follows keyword argument unpacking";
    } अण्णहा {
        सन्देशः = "positional argument follows keyword argument";
    }

    प्रत्यागम RAISE_SYNTAX_ERROR(सन्देशः);
}

शून्य *
_PyPegen_nonparen_genexp_in_call(Parser *p, व्यञ्जकम्_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    जइ (len <= 1) {
        प्रत्यागम रिक्त;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    प्रत्यागम RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (व्यञ्जकम्_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

स्थिर व्यञ्जकम्_ty
_PyPegen_decode_fstring_part(Parser* p, पूर्णाङ्क is_raw, व्यञ्जकम्_ty constant, Token* token) {
    प्रतिज्ञा(PyUnicode_CheckExact(constant->v.Constant.मानम्));

    ध्रुव वर्ण* bstr = PyUnicode_AsUTF8(constant->v.Constant.मानम्);
    जइ (bstr == रिक्त) {
        प्रत्यागम रिक्त;
    }

    size_t len;
    जइ (strcmp(bstr, "{{") == 0 || strcmp(bstr, "}}") == 0) {
        len = 1;
    } अण्णहा {
        len = strlen(bstr);
    }

    is_raw = is_raw || strchr(bstr, '\\') == रिक्त;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    जइ (str == रिक्त) {
        _Pypegen_raise_decode_error(p);
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

स्थिर asdl_व्यञ्जकम्_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions,
                   Token *b, पूर्णाङ्क is_tstring)
{
    Py_ssize_t n_वस्तूनि = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_वस्तूनि = n_वस्तूनि;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(raw_expressions, i);
        जइ (item->kind == JoinedStr_kind) {
            total_वस्तूनि += asdl_seq_LEN(item->v.JoinedStr.मानानि) - 1;
        }
    }

    ध्रुव वर्ण* quote_str = PyBytes_AsString(a->bytes);
    जइ (quote_str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    पूर्णाङ्क is_raw = strpbrk(quote_str, "rR") != रिक्त;

    asdl_व्यञ्जकम्_seq *seq = Py_asdl_व्यञ्जकम्_seq_new(total_वस्तूनि, p->arena);
    जइ (seq == रिक्त) {
        प्रत्यागम रिक्त;
    }

    Py_ssize_t index = 0;
    प्रति (Py_ssize_t i = 0; i < n_वस्तूनि; i++) {
        व्यञ्जकम्_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        जइ (item->kind == JoinedStr_kind) {
            asdl_व्यञ्जकम्_seq *मानानि = item->v.JoinedStr.मानानि;
            जइ (asdl_seq_LEN(मानानि) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                प्रत्यागम रिक्त;
            }

            व्यञ्जकम्_ty first = asdl_seq_GET(मानानि, 0);
            प्रतिज्ञा(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            व्यञ्जकम्_ty second = asdl_seq_GET(मानानि, 1);
            प्रतिज्ञा((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_मानम्_kind);
            asdl_seq_SET(seq, index++, second);

            अनुवर्त;
        }

        जइ (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            जइ (item == रिक्त) {
                प्रत्यागम रिक्त;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            जइ (PyUnicode_CheckExact(item->v.Constant.मानम्)
                && PyUnicode_GET_LENGTH(item->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_व्यञ्जकम्_seq *resized_exprs;
    जइ (index != total_वस्तूनि) {
        resized_exprs = Py_asdl_व्यञ्जकम्_seq_new(index, p->arena);
        जइ (resized_exprs == रिक्त) {
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    अण्णहा {
        resized_exprs = seq;
    }
    प्रत्यागम resized_exprs;
}

व्यञ्जकम्_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_व्यञ्जकम्_seq *raw_expressions, Token *b) {

    asdl_व्यञ्जकम्_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    जइ (resized_exprs == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

व्यञ्जकम्_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_व्यञ्जकम्_seq* raw_expressions, Token*b) {

    asdl_व्यञ्जकम्_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    जइ (resized_exprs == रिक्त) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

व्यञ्जकम्_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    जइ (tok->bytes == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    ध्रुव वर्ण *bstr = PyBytes_AS_STRING(tok->bytes);

    पूर्णाङ्क is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    जइ (str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

व्यञ्जकम्_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    जइ (tok->bytes == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    ध्रुव वर्ण *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    जइ (str == रिक्त) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

व्यञ्जकम्_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    जइ (tok->bytes == रिक्त) {
        प्रत्यागम Py_Pegen_raise_incomplete_निवेशः(p);
    }
    ध्रुव वर्ण *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    जइ (s == रिक्त) {
        _Pypegen_raise_decode_error(p);
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        प्रत्यागम रिक्त;
    }
    PyObject *kind = रिक्त;
    जइ (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        जइ (kind == रिक्त) {
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

स्थिर पूर्णाङ्क
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    जइ (conversion != रिक्त) {
        व्यञ्जकम्_ty conversion_व्यञ्जकम् = (व्यञ्जकम्_ty) conversion->result;
        प्रतिज्ञा(conversion_व्यञ्जकम्->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_व्यञ्जकम्->v.Name.id, 0);
        प्रत्यागम Py_SAFE_DOWNCAST(first, Py_UCS4, पूर्णाङ्क);
    }
    अण्णहा जइ (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        प्रत्यागम (पूर्णाङ्क)'r';
    }
    प्रत्यागम -1;
}

स्थिर PyObject *
strip_interpolation_debug_व्यञ्जकम्(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    जाव (len > 0) {
        पूर्णाङ्क has_newline = 0;
        जाव (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            जइ (!_PyUnicode_IsWhitespace(c)) {
                भङ्ग;
            }
            जइ (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        जइ (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            भङ्ग;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    जइ (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        प्रत्यागम Py_NewRef(exprstr);
    }

    प्रत्यागम PyUnicode_Substring(exprstr, 0, len - 1);
}

व्यञ्जकम्_ty _PyPegen_interpolation(Parser *p, व्यञ्जकम्_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                 पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena) {

    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_end_पङ्क्तिः, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    जइ (conversion) {
        debug_end_पङ्क्तिः = ((व्यञ्जकम्_ty) conversion->result)->lineno;
        debug_end_offset = ((व्यञ्जकम्_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    अण्णहा जइ (format) {
        debug_end_पङ्क्तिः = ((व्यञ्जकम्_ty) format->result)->lineno;
        debug_end_offset = ((व्यञ्जकम्_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    अण्णहा {
        debug_end_पङ्क्तिः = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    प्रतिज्ञा(exprstr != रिक्त);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_व्यञ्जकम्(exprstr)
        : Py_NewRef(exprstr);
    जइ (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        प्रत्यागम रिक्त;
    }

    व्यञ्जकम्_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (व्यञ्जकम्_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    जइ (!interpolation || !debug) {
        प्रत्यागम interpolation;
    }

    व्यञ्जकम्_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_पङ्क्तिः,
                                            debug_end_offset - 1, p->arena);
    जइ (!debug_text) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *मानानि = Py_asdl_व्यञ्जकम्_seq_new(2, arena);
    जइ (!मानानि) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_SET(मानानि, 0, debug_text);
    asdl_seq_SET(मानानि, 1, interpolation);
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_offset, debug_end_पङ्क्तिः, debug_end_offset, p->arena);
}

व्यञ्जकम्_ty Py_Pegen_formatted_मानम्(Parser *p, व्यञ्जकम्_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                 पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena) {
    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    व्यञ्जकम्_ty formatted_मानम् = Py_AST_Formatted_मानम्(
        expression, conversion_val, format ? (व्यञ्जकम्_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    जइ (!formatted_मानम् || !debug) {
        प्रत्यागम formatted_मानम्;
    }

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_end_पङ्क्तिः, debug_end_offset;
    PyObject *debug_metadata;

    जइ (conversion) {
        debug_end_पङ्क्तिः = ((व्यञ्जकम्_ty) conversion->result)->lineno;
        debug_end_offset = ((व्यञ्जकम्_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    अण्णहा जइ (format) {
        debug_end_पङ्क्तिः = ((व्यञ्जकम्_ty) format->result)->lineno;
        debug_end_offset = ((व्यञ्जकम्_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    अण्णहा {
        debug_end_पङ्क्तिः = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    व्यञ्जकम्_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_पङ्क्तिः,
                                            debug_end_offset - 1, p->arena);
    जइ (!debug_text) {
        प्रत्यागम रिक्त;
    }

    asdl_व्यञ्जकम्_seq *मानानि = Py_asdl_व्यञ्जकम्_seq_new(2, arena);
    जइ (!मानानि) {
        प्रत्यागम रिक्त;
    }
    asdl_seq_SET(मानानि, 0, debug_text);
    asdl_seq_SET(मानानि, 1, formatted_मानम्);
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_offset, debug_end_पङ्क्तिः, debug_end_offset, p->arena);
}

स्थिर व्यञ्जकम्_ty
_build_concatenated_bytes(Parser *p, asdl_व्यञ्जकम्_seq *strings, पूर्णाङ्क lineno,
                        पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    /* Bytes literals never get a kind, but just for consistency
        since they are represented as Constant nodes, we'll mirror
        the same behavior as unicode strings for determining the
        kind. */
    PyObject *kind = asdl_seq_GET(strings, 0)->v.Constant.kind;

    Py_ssize_t total = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मानम्;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        जइ (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            प्रत्यागम रिक्त;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    जइ (writer == रिक्त) {
        प्रत्यागम रिक्त;
    }
    वर्ण *out = PyBytesWriter_GetData(writer);

    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मानम्;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        जइ (part > 0) {
            memcpy(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    जइ (res == रिक्त) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

स्थिर व्यञ्जकम्_ty
_build_concatenated_unicode(Parser *p, asdl_व्यञ्जकम्_seq *strings, पूर्णाङ्क lineno,
                        पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 1);

    व्यञ्जकम्_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    जइ (writer == रिक्त) {
        प्रत्यागम रिक्त;
    }

    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty current_elem = asdl_seq_GET(strings, i);
        प्रतिज्ञा(current_elem->kind == Constant_kind);

        जइ (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.मानम्)) {
            PyUnicodeWriter_Discard(writer);
            प्रत्यागम रिक्त;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    जइ (final == रिक्त) {
        प्रत्यागम रिक्त;
    }
    जइ (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

स्थिर asdl_व्यञ्जकम्_seq *
_build_concatenated_str(Parser *p, asdl_व्यञ्जकम्_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.मानानि);
                भङ्ग;
            पक्ष TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.मानानि);
                भङ्ग;
            स्वभाव:
                n_flattened_elements++;
                भङ्ग;
        }
    }


    asdl_व्यञ्जकम्_seq* flattened = Py_asdl_व्यञ्जकम्_seq_new(n_flattened_elements, p->arena);
    जइ (flattened == रिक्त) {
        प्रत्यागम रिक्त;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    प्रति (Py_ssize_t i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष JoinedStr_kind:
                प्रति (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.मानानि); j++) {
                    व्यञ्जकम्_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.मानानि, j);
                    जइ (subvalue == रिक्त) {
                        प्रत्यागम रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                भङ्ग;
            पक्ष TemplateStr_kind:
                प्रति (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.मानानि); j++) {
                    व्यञ्जकम्_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.मानानि, j);
                    जइ (subvalue == रिक्त) {
                        प्रत्यागम रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                भङ्ग;
            स्वभाव:
                asdl_seq_SET(flattened, current_pos++, elem);
                भङ्ग;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    पूर्णाङ्क prev_is_constant = 0;
    प्रति (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        जइ (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.मानम्) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.मानम्) == 0)
            अनुवर्त;

        जइ (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_व्यञ्जकम्_seq* मानानि = Py_asdl_व्यञ्जकम्_seq_new(n_elements, p->arena);
    जइ (मानानि == रिक्त) {
        प्रत्यागम रिक्त;
    }

    /* build folded list */
    current_pos = 0;
    प्रति (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        जइ (elem->kind == Constant_kind) {
            जइ (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                व्यञ्जकम्_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                जइ (writer == रिक्त) {
                    प्रत्यागम रिक्त;
                }
                व्यञ्जकम्_ty last_elem = elem;
                Py_ssize_t j;
                प्रति (j = i; j < n_flattened_elements; j++) {
                    व्यञ्जकम्_ty current_elem = asdl_seq_GET(flattened, j);
                    जइ (current_elem->kind == Constant_kind) {
                        जइ (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.मानम्)) {
                            PyUnicodeWriter_Discard(writer);
                            प्रत्यागम रिक्त;
                        }
                        last_elem = current_elem;
                    } अण्णहा {
                        भङ्ग;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                जइ (concat_str == रिक्त) {
                    प्रत्यागम रिक्त;
                }
                जइ (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    प्रत्यागम रिक्त;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                जइ (elem == रिक्त) {
                    प्रत्यागम रिक्त;
                }
            }

            /* Drop all empty contanst strings */
            जइ (PyUnicode_CheckExact(elem->v.Constant.मानम्) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.मानम्) == 0) {
                अनुवर्त;
            }
        }

        asdl_seq_SET(मानानि, current_pos++, elem);
    }

    प्रतिज्ञा(current_pos == n_elements);
    प्रत्यागम मानानि;
}

स्थिर व्यञ्जकम्_ty
_build_concatenated_joined_str(Parser *p, asdl_व्यञ्जकम्_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    asdl_व्यञ्जकम्_seq *मानानि = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    जइ (!मानानि) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_JoinedStr(मानानि, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

व्यञ्जकम्_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_व्यञ्जकम्_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    asdl_व्यञ्जकम्_seq *मानानि = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    जइ (!मानानि) {
        प्रत्यागम रिक्त;
    }
    प्रत्यागम _PyAST_TemplateStr(मानानि, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

व्यञ्जकम्_ty
_PyPegen_concatenate_strings(Parser *p, asdl_व्यञ्जकम्_seq *strings,
                             पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                             पूर्णाङ्क end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    पूर्णाङ्क f_string_found = 0;
    पूर्णाङ्क unicode_string_found = 0;
    पूर्णाङ्क bytes_found = 0;

    Py_ssize_t i = 0;
    प्रति (i = 0; i < len; i++) {
        व्यञ्जकम्_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष Constant_kind:
                जइ (PyBytes_CheckExact(elem->v.Constant.मानम्)) {
                    bytes_found = 1;
                } अण्णहा {
                    unicode_string_found = 1;
                }
                भङ्ग;
            पक्ष JoinedStr_kind:
                f_string_found = 1;
                भङ्ग;
            पक्ष TemplateStr_kind:
                // python.gram handles this; we should never get here
                प्रतिज्ञा(0);
                भङ्ग;
            स्वभाव:
                f_string_found = 1;
                भङ्ग;
        }
    }

    // Cannot mix unicode and bytes
    जइ ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        प्रत्यागम रिक्त;
    }

    // If it's only bytes or only unicode string, do a simple concat
    जइ (!f_string_found) {
        जइ (len == 1) {
            प्रत्यागम asdl_seq_GET(strings, 0);
        }
        अण्णहा जइ (bytes_found) {
            प्रत्यागम _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        अण्णहा {
            प्रत्यागम _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    प्रत्यागम _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

स्थिर पूर्णाङ्क
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    जइ (p->call_invalid_rules) {
        प्रत्यागम 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    जइ (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        प्रत्यागम 0;
    }

    पूर्णाङ्क गणनम् = Py_Pegen_seq_गणनम्_dots(dots);
    वर्ण *buf = PyMem_RawMalloc(गणनम् + 1);
    जइ (buf == रिक्त) {
        PyErr_NoMemory();
        प्रत्यागम -1;
    }
    memset(buf, '.', गणनम्);
    buf[गणनम्] = '\0';

    PyObject *सन्देशः = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    जइ (सन्देशः == रिक्त) {
        प्रत्यागम -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    पूर्णाङ्क res = _PyErr_EmitSyntaxWarning(सन्देशः,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(सन्देशः);
    प्रत्यागम res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, व्यञ्जकम्_ty module_name,
                             asdl_alias_seq *names, व्यञ्जकम्_ty lazy_token, पूर्णाङ्क lineno,
                             पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    पूर्णाङ्क level = Py_Pegen_seq_गणनम्_dots(dots);
    जइ (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        जइ (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            प्रत्यागम रिक्त;
        }
        प्रति (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            जइ (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    अण्णहा जइ (
        level > 0
        && lazy_token == रिक्त
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        जइ (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            प्रत्यागम रिक्त;
        }
    }
    प्रत्यागम _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    जइ (!p->call_invalid_rules) {
        प्रत्यागम stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    जइ (len == 0) {
        प्रत्यागम stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    जइ (p->last_stmt_location.lineno > last_stmt->lineno) {
        प्रत्यागम stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    प्रत्यागम stmts;
}
