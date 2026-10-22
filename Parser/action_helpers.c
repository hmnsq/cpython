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
    फर्काऊ &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    यदि (!seq) {
        फर्काऊ रिक्त;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    फर्काऊ seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_घुसाउ_in_front(Parser *p, शून्य *a, asdl_seq *seq)
{
    प्रतिज्ञा(a != रिक्त);
    यदि (!seq) {
        फर्काऊ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    लागि (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    फर्काऊ new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    यदि (!seq) {
        फर्काऊ _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }

    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    फर्काऊ new_seq;
}

स्थिर Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    फर्काऊ size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    प्रतिज्ञा(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    यदि (!flattened_seq) {
        फर्काऊ रिक्त;
    }

    पूर्णाङ्क flattened_seq_idx = 0;
    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        लागि (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    प्रतिज्ञा(flattened_seq_idx == flattened_seq_size);

    फर्काऊ flattened_seq;
}

शून्य *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    फर्काऊ asdl_seq_GET_UNTYPED(seq, len - 1);
}

शून्य *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    फर्काऊ asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
अभिव्यक्ति_ty
_PyPegen_join_names_with_dot(Parser *p, अभिव्यक्ति_ty first_name, अभिव्यक्ति_ty second_name)
{
    प्रतिज्ञा(first_name != रिक्त && second_name != रिक्त);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    यदि (!uni) {
        फर्काऊ रिक्त;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    यदि (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        फर्काऊ रिक्त;
    }

    फर्काऊ _PyAST_Name(uni, Load, EXTRA_अभिव्यक्ति(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
पूर्णाङ्क
Py_Pegen_seq_गन्ती_dots(asdl_seq *seq)
{
    पूर्णाङ्क सङ्ख्या_of_dots = 0;
    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_अभिव्यक्ति = asdl_seq_GET_UNTYPED(seq, i);
        विकल्प (current_अभिव्यक्ति->type) {
            पक्ष ELLIPSIS:
                सङ्ख्या_of_dots += 3;
                रोक;
            पक्ष DOT:
                सङ्ख्या_of_dots += 1;
                रोक;
            स्वभाव:
                Py_UNREACHABLE();
        }
    }

    फर्काऊ सङ्ख्या_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                        पूर्णाङ्क end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    यदि (!str) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_alias(str, रिक्त, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_अभिव्यक्ति_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    फर्काऊ new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_अभिव्यक्ति_Pair *
Py_Pegen_cmpop_अभिव्यक्ति_pair(Parser *p, cmpop_ty cmpop, अभिव्यक्ति_ty अभिव्यक्ति)
{
    प्रतिज्ञा(अभिव्यक्ति != रिक्त);
    Cmpop_अभिव्यक्ति_Pair *a = _PyArena_Malloc(p->arena, परिमाण(Cmpop_अभिव्यक्ति_Pair));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->cmpop = cmpop;
    a->अभिव्यक्ति = अभिव्यक्ति;
    फर्काऊ a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_अभिव्यक्ति_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    फर्काऊ new_seq;
}

asdl_अभिव्यक्ति_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_अभिव्यक्ति_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->अभिव्यक्ति);
    }
    फर्काऊ new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
स्थिर asdl_अभिव्यक्ति_seq *
_set_seq_context(Parser *p, asdl_अभिव्यक्ति_seq *seq, अभिव्यक्ति_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    यदि (len == 0) {
        फर्काऊ रिक्त;
    }

    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty e = asdl_seq_GET(seq, i);
        अभिव्यक्ति_ty new_e = Py_Pegen_set_अभिव्यक्ति_context(p, e, ctx);
        यदि (!new_e) {
            फर्काऊ रिक्त;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    फर्काऊ new_seq;
}

स्थिर अभिव्यक्ति_ty
_set_name_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    फर्काऊ _PyAST_Name(e->v.Name.id, ctx, EXTRA_अभिव्यक्ति(e, e));
}

स्थिर अभिव्यक्ति_ty
_set_tuple_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    asdl_अभिव्यक्ति_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    यदि (!seq && PyErr_Occurred()) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Tuple(seq, ctx, EXTRA_अभिव्यक्ति(e, e));
}

स्थिर अभिव्यक्ति_ty
_set_list_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    asdl_अभिव्यक्ति_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    यदि (!seq && PyErr_Occurred()) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_List(seq, ctx, EXTRA_अभिव्यक्ति(e, e));
}

स्थिर अभिव्यक्ति_ty
_set_subscript_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    फर्काऊ _PyAST_Subscript(e->v.Subscript.मान, e->v.Subscript.slice,
                            ctx, EXTRA_अभिव्यक्ति(e, e));
}

स्थिर अभिव्यक्ति_ty
_set_attribute_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    फर्काऊ _PyAST_Attribute(e->v.Attribute.मान, e->v.Attribute.attr,
                            ctx, EXTRA_अभिव्यक्ति(e, e));
}

स्थिर अभिव्यक्ति_ty
_set_starred_context(Parser *p, अभिव्यक्ति_ty e, अभिव्यक्ति_context_ty ctx)
{
    अभिव्यक्ति_ty inner = Py_Pegen_set_अभिव्यक्ति_context(p, e->v.Starred.मान, ctx);
    यदि (!inner) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Starred(inner, ctx, EXTRA_अभिव्यक्ति(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
अभिव्यक्ति_ty
Py_Pegen_set_अभिव्यक्ति_context(Parser *p, अभिव्यक्ति_ty अभिव्यक्ति, अभिव्यक्ति_context_ty ctx)
{
    प्रतिज्ञा(अभिव्यक्ति != रिक्त);

    अभिव्यक्ति_ty new = रिक्त;
    विकल्प (अभिव्यक्ति->kind) {
        पक्ष Name_kind:
            new = _set_name_context(p, अभिव्यक्ति, ctx);
            रोक;
        पक्ष Tuple_kind:
            new = _set_tuple_context(p, अभिव्यक्ति, ctx);
            रोक;
        पक्ष List_kind:
            new = _set_list_context(p, अभिव्यक्ति, ctx);
            रोक;
        पक्ष Subscript_kind:
            new = _set_subscript_context(p, अभिव्यक्ति, ctx);
            रोक;
        पक्ष Attribute_kind:
            new = _set_attribute_context(p, अभिव्यक्ति, ctx);
            रोक;
        पक्ष Starred_kind:
            new = _set_starred_context(p, अभिव्यक्ति, ctx);
            रोक;
        स्वभाव:
            new = अभिव्यक्ति;
    }
    फर्काऊ new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
अभिव्यक्ति_ty
_PyPegen_make_target(Parser *p, अभिव्यक्ति_ty अभिव्यक्ति, TARGETS_TYPE targets_type)
{
    प्रतिज्ञा(अभिव्यक्ति != रिक्त);
    प्रतिज्ञा(targets_type != FOR_TARGETS);
    यदि (_PyPegen_get_invalid_target(अभिव्यक्ति, targets_type) != रिक्त) {
        फर्काऊ रिक्त;
    }
    फर्काऊ Py_Pegen_set_अभिव्यक्ति_context(
        p, अभिव्यक्ति, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
साँचो_मान_Pair *
Py_Pegen_साँचो_मान_pair(Parser *p, अभिव्यक्ति_ty साँचो, अभिव्यक्ति_ty मान)
{
    साँचो_मान_Pair *a = _PyArena_Malloc(p->arena, परिमाण(साँचो_मान_Pair));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->साँचो = साँचो;
    a->मान = मान;
    फर्काऊ a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_अभिव्यक्ति_seq *
Py_Pegen_get_साँचोहरू(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        साँचो_मान_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->साँचो);
    }
    फर्काऊ new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_अभिव्यक्ति_seq *
Py_Pegen_get_मानहरू(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        साँचो_मान_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->मान);
    }
    फर्काऊ new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
साँचो_Pattern_Pair *
Py_Pegen_साँचो_pattern_pair(Parser *p, अभिव्यक्ति_ty साँचो, pattern_ty pattern)
{
    साँचो_Pattern_Pair *a = _PyArena_Malloc(p->arena, परिमाण(साँचो_Pattern_Pair));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->साँचो = साँचो;
    a->pattern = pattern;
    फर्काऊ a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_अभिव्यक्ति_seq *
Py_Pegen_get_pattern_साँचोहरू(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        साँचो_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->साँचो);
    }
    फर्काऊ new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        साँचो_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    फर्काऊ new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, अभिव्यक्ति_ty मान, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, परिमाण(NameDefaultPair));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    यदि (!a->arg) {
        फर्काऊ रिक्त;
    }
    a->मान = मान;
    फर्काऊ a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, परिमाण(SlashWithDefault));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    फर्काऊ a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, परिमाण(StarEtc));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    फर्काऊ a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }

    पूर्णाङ्क k = 0;
    लागि (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    लागि (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    फर्काऊ new_seq;
}

स्थिर asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    यदि (!seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    फर्काऊ seq;
}

स्थिर asdl_अभिव्यक्ति_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_अभिव्यक्ति_seq *seq = Py_asdl_अभिव्यक्ति_seq_new(len, p->arena);
    यदि (!seq) {
        फर्काऊ रिक्त;
    }
    लागि (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->मान);
    }
    फर्काऊ seq;
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
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        यदि (!slash_with_default_names) {
            फर्काऊ -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    अन्यथा {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    फर्काऊ *posonlyargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    यदि (names_with_default != रिक्त) {
        यदि (plain_names != रिक्त) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            यदि (!names_with_default_names) {
                फर्काऊ -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        अन्यथा {
            *posargs = _get_names(p, names_with_default);
        }
    }
    अन्यथा {
        यदि (plain_names != रिक्त) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            प्रतिज्ञा(0);
            *posargs = plain_names;
        }
        अन्यथा {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    फर्काऊ *posargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_अभिव्यक्ति_seq **posdefaults) {
    यदि (slash_with_default != रिक्त && names_with_default != रिक्त) {
        asdl_अभिव्यक्ति_seq *slash_with_default_मानहरू =
                _get_defaults(p, slash_with_default->names_with_defaults);
        यदि (!slash_with_default_मानहरू) {
            फर्काऊ -1;
        }
        asdl_अभिव्यक्ति_seq *names_with_default_मानहरू = _get_defaults(p, names_with_default);
        यदि (!names_with_default_मानहरू) {
            फर्काऊ -1;
        }
        *posdefaults = (asdl_अभिव्यक्ति_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_मानहरू,
                (asdl_seq*)names_with_default_मानहरू);
    }
    अन्यथा यदि (slash_with_default == रिक्त && names_with_default != रिक्त) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    अन्यथा यदि (slash_with_default != रिक्त && names_with_default == रिक्त) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    अन्यथा {
        *posdefaults = Py_asdl_अभिव्यक्ति_seq_new(0, p->arena);
    }
    फर्काऊ *posdefaults == रिक्त ? -1 : 0;
}

स्थिर पूर्णाङ्क
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_अभिव्यक्ति_seq **kwdefaults) {
    यदि (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    अन्यथा {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    यदि (*kwonlyargs == रिक्त) {
        फर्काऊ -1;
    }

    यदि (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    अन्यथा {
        *kwdefaults = Py_asdl_अभिव्यक्ति_seq_new(0, p->arena);
    }

    यदि (*kwdefaults == रिक्त) {
        फर्काऊ -1;
    }

    फर्काऊ 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    यदि (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        फर्काऊ रिक्त;
    }

    asdl_arg_seq *posargs;
    यदि (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        फर्काऊ रिक्त;
    }

    asdl_अभिव्यक्ति_seq *posdefaults;
    यदि (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        फर्काऊ रिक्त;
    }

    arg_ty vararg = रिक्त;
    यदि (star_etc != रिक्त && star_etc->vararg != रिक्त) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_अभिव्यक्ति_seq *kwdefaults;
    यदि (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        फर्काऊ रिक्त;
    }

    arg_ty kwarg = रिक्त;
    यदि (star_etc != रिक्त && star_etc->kwarg != रिक्त) {
        kwarg = star_etc->kwarg;
    }

    फर्काऊ _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!posonlyargs) {
        फर्काऊ रिक्त;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!posargs) {
        फर्काऊ रिक्त;
    }
    asdl_अभिव्यक्ति_seq *posdefaults = Py_asdl_अभिव्यक्ति_seq_new(0, p->arena);
    यदि (!posdefaults) {
        फर्काऊ रिक्त;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    यदि (!kwonlyargs) {
        फर्काऊ रिक्त;
    }
    asdl_अभिव्यक्ति_seq *kwdefaults = Py_asdl_अभिव्यक्ति_seq_new(0, p->arena);
    यदि (!kwdefaults) {
        फर्काऊ रिक्त;
    }

    फर्काऊ _PyAST_arguments(posonlyargs, posargs, रिक्त, kwonlyargs,
                            kwdefaults, रिक्त, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, परिमाण(AugOperator));
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->kind = kind;
    फर्काऊ a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_अभिव्यक्ति_seq *decorators, stmt_ty function_def)
{
    प्रतिज्ञा(function_def != रिक्त);
    यदि (function_def->kind == AsyncFunctionDef_kind) {
        फर्काऊ _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    फर्काऊ _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_अभिव्यक्ति_seq *decorators, stmt_ty class_def)
{
    प्रतिज्ञा(class_def != रिक्त);
    फर्काऊ _PyAST_ClassDef(
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
    यदि (!a) {
        फर्काऊ रिक्त;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    फर्काऊ a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
स्थिर पूर्णाङ्क
seq_सङ्ख्या_of_starred_exprs(asdl_seq *seq)
{
    पूर्णाङ्क n = 0;
    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        यदि (!k->is_keyword) {
            n++;
        }
    }
    फर्काऊ n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_अभिव्यक्ति_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    पूर्णाङ्क new_len = seq_सङ्ख्या_of_starred_exprs(kwargs);
    यदि (new_len == 0) {
        फर्काऊ रिक्त;
    }
    asdl_अभिव्यक्ति_seq *new_seq = Py_asdl_अभिव्यक्ति_seq_new(new_len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }

    पूर्णाङ्क idx = 0;
    लागि (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        यदि (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    फर्काऊ new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_सङ्ख्या_of_starred_exprs(kwargs);
    यदि (new_len == 0) {
        फर्काऊ रिक्त;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    यदि (!new_seq) {
        फर्काऊ रिक्त;
    }

    पूर्णाङ्क idx = 0;
    लागि (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        यदि (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    फर्काऊ new_seq;
}

अभिव्यक्ति_ty
_PyPegen_ensure_imaginary(Parser *p, अभिव्यक्ति_ty exp)
{
    यदि (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.मान)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        फर्काऊ रिक्त;
    }
    फर्काऊ exp;
}

अभिव्यक्ति_ty
_PyPegen_ensure_real(Parser *p, अभिव्यक्ति_ty exp)
{
    यदि (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.मान)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        फर्काऊ रिक्त;
    }
    फर्काऊ exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = रिक्त;
    Py_ssize_t अङ्क = p->type_ignore_comments.अङ्क_वस्तुहरू;
    यदि (अङ्क > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(अङ्क, p->arena);
        यदि (type_ignores == रिक्त) {
            फर्काऊ रिक्त;
        }
        लागि (Py_ssize_t i = 0; i < अङ्क; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.वस्तुहरू[i].comment);
            यदि (tag == रिक्त) {
                फर्काऊ रिक्त;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.वस्तुहरू[i].lineno,
                                                  tag, p->arena);
            यदि (ti == रिक्त) {
                फर्काऊ रिक्त;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    फर्काऊ _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, अचर वर्ण *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, म_माप(s), रिक्त);
    यदि (res == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        फर्काऊ रिक्त;
    }
    फर्काऊ res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    यदि (tc == रिक्त) {
        फर्काऊ a;
    }
    अचर वर्ण *bytes = PyBytes_AsString(tc->bytes);
    यदि (bytes == रिक्त) {
        फर्काऊ रिक्त;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    यदि (tco == रिक्त) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
पूर्णाङ्क
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    प्रतिज्ञा(t->bytes != रिक्त);
    प्रतिज्ञा(t->type == NOTEQUAL);

    अचर वर्ण* tok_str = PyBytes_AS_STRING(t->bytes);
    यदि (p->flags & PyPARSE_BARRY_AS_BDFL && म_भेद(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        फर्काऊ -1;
    }
    यदि (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        फर्काऊ म_भेद(tok_str, "!=");
    }
    फर्काऊ 0;
}

पूर्णाङ्क
_PyPegen_check_legacy_stmt(Parser *p, अभिव्यक्ति_ty name) {
    यदि (name->kind != Name_kind) {
        फर्काऊ 0;
    }
    अचर वर्ण* candidates[2] = {"print", "exec"};
    लागि (पूर्णाङ्क i=0; i<2; i++) {
        यदि (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            फर्काऊ 1;
        }
    }
    फर्काऊ 0;
}

शून्य *
_PyPegen_raise_error_for_missing_comma(Parser *p, अभिव्यक्ति_ty a, अभिव्यक्ति_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    यदि (_PyPegen_check_legacy_stmt(p, a)) {
        फर्काऊ रिक्त;
    }
    // Only raise inside parentheses/brackets (level > 0)
    यदि (p->tokens[p->mark - 1]->level == 0) {
        फर्काऊ रिक्त;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    यदि (a->end_lineno > a->lineno) {
        फर्काऊ RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    फर्काऊ RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

स्थिर ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, शून्य *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, परिमाण(ResultTokenWithMetadata));
    यदि (res == रिक्त) {
        फर्काऊ रिक्त;
    }
    res->metadata = metadata;
    res->result = result;
    फर्काऊ res;
}

स्थिर वर्ण
formatted_string_prefix(अचर Parser *p)
{
    पूर्णाङ्क nested = 0;
    लागि (पूर्णाङ्क i = p->mark - 1; i >= 0; i--) {
        पूर्णाङ्क type = p->tokens[i]->type;
        यदि (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        अन्यथा यदि (type == FSTRING_START || type == TSTRING_START) {
            यदि (nested == 0) {
                फर्काऊ type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, अभिव्यक्ति_ty conv)
{
    यदि (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        फर्काऊ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    यदि (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        फर्काऊ रिक्त;
    }

    फर्काऊ result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_अभिव्यक्ति_seq *spec, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena)
{
    यदि (!spec) {
        फर्काऊ रिक्त;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_वस्तुहरू = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_गन्ती = 0;
    लागि (Py_ssize_t i = 0; i < n_वस्तुहरू; i++) {
        अभिव्यक्ति_ty item = asdl_seq_GET(spec, i);
        non_empty_गन्ती += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.मान) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.मान) == 0);
    }
    यदि (non_empty_गन्ती != n_वस्तुहरू) {
        asdl_अभिव्यक्ति_seq *resized_spec =
            Py_asdl_अभिव्यक्ति_seq_new(non_empty_गन्ती, p->arena);
        यदि (resized_spec == रिक्त) {
            फर्काऊ रिक्त;
        }
        Py_ssize_t j = 0;
        लागि (Py_ssize_t i = 0; i < n_वस्तुहरू; i++) {
            अभिव्यक्ति_ty item = asdl_seq_GET(spec, i);
            यदि (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.मान) &&
                PyUnicode_GET_LENGTH(item->v.Constant.मान) == 0) {
                जारी;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        प्रतिज्ञा(j == non_empty_गन्ती);
        spec = resized_spec;
    }
    अभिव्यक्ति_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    यदि (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } अन्यथा {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    यदि (!res) {
        फर्काऊ रिक्त;
    }
    फर्काऊ result_token_with_metadata(p, res, colon->metadata);
}

अचर वर्ण *
Py_Pegen_get_अभिव्यक्ति_name(अभिव्यक्ति_ty e)
{
    प्रतिज्ञा(e != रिक्त);
    विकल्प (e->kind) {
        पक्ष Attribute_kind:
            फर्काऊ "attribute";
        पक्ष Subscript_kind:
            फर्काऊ "subscript";
        पक्ष Starred_kind:
            फर्काऊ "starred";
        पक्ष Name_kind:
            फर्काऊ "name";
        पक्ष List_kind:
            फर्काऊ "list";
        पक्ष Tuple_kind:
            फर्काऊ "tuple";
        पक्ष Lambda_kind:
            फर्काऊ "lambda";
        पक्ष Call_kind:
            फर्काऊ "function call";
        पक्ष BoolOp_kind:
        पक्ष BinOp_kind:
        पक्ष UnaryOp_kind:
            फर्काऊ "expression";
        पक्ष GeneratorExp_kind:
            फर्काऊ "generator expression";
        पक्ष Yield_kind:
        पक्ष YieldFrom_kind:
            फर्काऊ "yield expression";
        पक्ष Await_kind:
            फर्काऊ "await expression";
        पक्ष ListComp_kind:
            फर्काऊ "list comprehension";
        पक्ष SetComp_kind:
            फर्काऊ "set comprehension";
        पक्ष DictComp_kind:
            फर्काऊ "dict comprehension";
        पक्ष Dict_kind:
            फर्काऊ "dict literal";
        पक्ष Set_kind:
            फर्काऊ "set display";
        पक्ष JoinedStr_kind:
        पक्ष Formatted_मान_kind:
            फर्काऊ "f-string expression";
        पक्ष TemplateStr_kind:
        पक्ष Interpolation_kind:
            फर्काऊ "t-string expression";
        पक्ष Constant_kind: {
            PyObject *मान = e->v.Constant.मान;
            यदि (मान == Py_None) {
                फर्काऊ "None";
            }
            यदि (मान == Py_False) {
                फर्काऊ "False";
            }
            यदि (मान == Py_True) {
                फर्काऊ "True";
            }
            यदि (मान == Py_Ellipsis) {
                फर्काऊ "ellipsis";
            }
            फर्काऊ "literal";
        }
        पक्ष Compare_kind:
            फर्काऊ "comparison";
        पक्ष IfExp_kind:
            फर्काऊ "conditional expression";
        पक्ष Named_अभिव्यक्ति_kind:
            फर्काऊ "named expression";
        स्वभाव:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            फर्काऊ रिक्त;
    }
}

अभिव्यक्ति_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    यदि (comprehension->ifs == रिक्त || asdl_seq_LEN(comprehension->ifs) == 0) {
        फर्काऊ comprehension->iter;
    }
    फर्काऊ PyPegen_last_item(comprehension->ifs, अभिव्यक्ति_ty);
}

अभिव्यक्ति_ty _PyPegen_collect_call_seqs(Parser *p, asdl_अभिव्यक्ति_seq *a, asdl_seq *b,
                     पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                     पूर्णाङ्क end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    यदि (b == रिक्त) {
        फर्काऊ _PyAST_Call(_PyPegen_dummy_name(p), a, रिक्त, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_अभिव्यक्ति_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    यदि (!starreds && PyErr_Occurred()) {
        फर्काऊ रिक्त;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    यदि (!keywords && PyErr_Occurred()) {
        फर्काऊ रिक्त;
    }

    यदि (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_अभिव्यक्ति_seq *args = Py_asdl_अभिव्यक्ति_seq_new(total_len, arena);
    यदि (args == रिक्त) {
        फर्काऊ रिक्त;
    }

    Py_ssize_t i = 0;
    लागि (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    लागि (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    फर्काऊ _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

अभिव्यक्ति_ty
_PyPegen_get_invalid_target(अभिव्यक्ति_ty e, TARGETS_TYPE targets_type)
{
    यदि (e == रिक्त) {
        फर्काऊ रिक्त;
    }

    यदि (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        यदि (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            फर्काऊ रिक्त;
        }
        फर्काऊ e->kind == Attribute_kind || e->kind == Subscript_kind ? रिक्त : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) गर { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        लागि (Py_ssize_t i = 0; i < len; i++) {\
            अभिव्यक्ति_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            अभिव्यक्ति_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            यदि (child != रिक्त) {\
                फर्काऊ child;\
            }\
        }\
    } जबसम्म (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    विकल्प (e->kind) {
        पक्ष List_kind:
            VISIT_CONTAINER(e, List);
            फर्काऊ रिक्त;
        पक्ष Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            फर्काऊ रिक्त;
        पक्ष Starred_kind:
            यदि (targets_type == DEL_TARGETS) {
                फर्काऊ e;
            }
            फर्काऊ _PyPegen_get_invalid_target(e->v.Starred.मान, targets_type);
        पक्ष Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            यदि (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                यदि (cmpop == In) {
                    फर्काऊ _PyPegen_get_invalid_target(e->v.Compare.बायाँ, targets_type);
                }
                फर्काऊ रिक्त;
            }
            फर्काऊ e;
        पक्ष Name_kind:
        पक्ष Subscript_kind:
        पक्ष Attribute_kind:
            फर्काऊ रिक्त;
        स्वभाव:
            फर्काऊ e;
    }
}

शून्य *_PyPegen_arguments_parsing_error(Parser *p, अभिव्यक्ति_ty e) {
    पूर्णाङ्क kwarg_unpacking = 0;
    लागि (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        यदि (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    अचर वर्ण *सन्देश = रिक्त;
    यदि (kwarg_unpacking) {
        सन्देश = "positional argument follows keyword argument unpacking";
    } अन्यथा {
        सन्देश = "positional argument follows keyword argument";
    }

    फर्काऊ RAISE_SYNTAX_ERROR(सन्देश);
}

शून्य *
_PyPegen_nonparen_genexp_in_call(Parser *p, अभिव्यक्ति_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    यदि (len <= 1) {
        फर्काऊ रिक्त;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    फर्काऊ RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (अभिव्यक्ति_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

स्थिर अभिव्यक्ति_ty
_PyPegen_decode_fstring_part(Parser* p, पूर्णाङ्क is_raw, अभिव्यक्ति_ty constant, Token* token) {
    प्रतिज्ञा(PyUnicode_CheckExact(constant->v.Constant.मान));

    अचर वर्ण* bstr = PyUnicode_AsUTF8(constant->v.Constant.मान);
    यदि (bstr == रिक्त) {
        फर्काऊ रिक्त;
    }

    माप_प्रकार len;
    यदि (म_भेद(bstr, "{{") == 0 || म_भेद(bstr, "}}") == 0) {
        len = 1;
    } अन्यथा {
        len = म_माप(bstr);
    }

    is_raw = is_raw || म_अक्षर(bstr, '\\') == रिक्त;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    यदि (str == रिक्त) {
        _Pypegen_raise_decode_error(p);
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Constant(str, रिक्त, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

स्थिर asdl_अभिव्यक्ति_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_अभिव्यक्ति_seq *raw_expressions,
                   Token *b, पूर्णाङ्क is_tstring)
{
    Py_ssize_t n_वस्तुहरू = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_वस्तुहरू = n_वस्तुहरू;
    लागि (Py_ssize_t i = 0; i < n_वस्तुहरू; i++) {
        अभिव्यक्ति_ty item = asdl_seq_GET(raw_expressions, i);
        यदि (item->kind == JoinedStr_kind) {
            total_वस्तुहरू += asdl_seq_LEN(item->v.JoinedStr.मानहरू) - 1;
        }
    }

    अचर वर्ण* quote_str = PyBytes_AsString(a->bytes);
    यदि (quote_str == रिक्त) {
        फर्काऊ रिक्त;
    }
    पूर्णाङ्क is_raw = strpbrk(quote_str, "rR") != रिक्त;

    asdl_अभिव्यक्ति_seq *seq = Py_asdl_अभिव्यक्ति_seq_new(total_वस्तुहरू, p->arena);
    यदि (seq == रिक्त) {
        फर्काऊ रिक्त;
    }

    Py_ssize_t index = 0;
    लागि (Py_ssize_t i = 0; i < n_वस्तुहरू; i++) {
        अभिव्यक्ति_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        यदि (item->kind == JoinedStr_kind) {
            asdl_अभिव्यक्ति_seq *मानहरू = item->v.JoinedStr.मानहरू;
            यदि (asdl_seq_LEN(मानहरू) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                फर्काऊ रिक्त;
            }

            अभिव्यक्ति_ty first = asdl_seq_GET(मानहरू, 0);
            प्रतिज्ञा(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            अभिव्यक्ति_ty second = asdl_seq_GET(मानहरू, 1);
            प्रतिज्ञा((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_मान_kind);
            asdl_seq_SET(seq, index++, second);

            जारी;
        }

        यदि (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            यदि (item == रिक्त) {
                फर्काऊ रिक्त;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            यदि (PyUnicode_CheckExact(item->v.Constant.मान)
                && PyUnicode_GET_LENGTH(item->v.Constant.मान) == 0) {
                जारी;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_अभिव्यक्ति_seq *resized_exprs;
    यदि (index != total_वस्तुहरू) {
        resized_exprs = Py_asdl_अभिव्यक्ति_seq_new(index, p->arena);
        यदि (resized_exprs == रिक्त) {
            फर्काऊ रिक्त;
        }
        लागि (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    अन्यथा {
        resized_exprs = seq;
    }
    फर्काऊ resized_exprs;
}

अभिव्यक्ति_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_अभिव्यक्ति_seq *raw_expressions, Token *b) {

    asdl_अभिव्यक्ति_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    यदि (resized_exprs == रिक्त) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

अभिव्यक्ति_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_अभिव्यक्ति_seq* raw_expressions, Token*b) {

    asdl_अभिव्यक्ति_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    यदि (resized_exprs == रिक्त) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

अभिव्यक्ति_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    यदि (tok->bytes == रिक्त) {
        फर्काऊ Py_Pegen_raise_incomplete_आगत(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    अचर वर्ण *bstr = PyBytes_AS_STRING(tok->bytes);

    पूर्णाङ्क is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    यदि (str == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

अभिव्यक्ति_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    यदि (tok->bytes == रिक्त) {
        फर्काऊ Py_Pegen_raise_incomplete_आगत(p);
    }
    अचर वर्ण *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    यदि (str == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

अभिव्यक्ति_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    यदि (tok->bytes == रिक्त) {
        फर्काऊ Py_Pegen_raise_incomplete_आगत(p);
    }
    अचर वर्ण *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    यदि (s == रिक्त) {
        _Pypegen_raise_decode_error(p);
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        फर्काऊ रिक्त;
    }
    PyObject *kind = रिक्त;
    यदि (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        यदि (kind == रिक्त) {
            फर्काऊ रिक्त;
        }
    }
    फर्काऊ _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

स्थिर पूर्णाङ्क
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    यदि (conversion != रिक्त) {
        अभिव्यक्ति_ty conversion_अभिव्यक्ति = (अभिव्यक्ति_ty) conversion->result;
        प्रतिज्ञा(conversion_अभिव्यक्ति->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_अभिव्यक्ति->v.Name.id, 0);
        फर्काऊ Py_SAFE_DOWNCAST(first, Py_UCS4, पूर्णाङ्क);
    }
    अन्यथा यदि (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        फर्काऊ (पूर्णाङ्क)'r';
    }
    फर्काऊ -1;
}

स्थिर PyObject *
strip_interpolation_debug_अभिव्यक्ति(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    जबसम्म (len > 0) {
        पूर्णाङ्क has_newline = 0;
        जबसम्म (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            यदि (!_PyUnicode_IsWhitespace(c)) {
                रोक;
            }
            यदि (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        यदि (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            रोक;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    यदि (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        फर्काऊ Py_NewRef(exprstr);
    }

    फर्काऊ PyUnicode_Substring(exprstr, 0, len - 1);
}

अभिव्यक्ति_ty _PyPegen_interpolation(Parser *p, अभिव्यक्ति_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                 पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena) {

    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_end_पङ्क्ति, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    यदि (conversion) {
        debug_end_पङ्क्ति = ((अभिव्यक्ति_ty) conversion->result)->lineno;
        debug_end_offset = ((अभिव्यक्ति_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    अन्यथा यदि (format) {
        debug_end_पङ्क्ति = ((अभिव्यक्ति_ty) format->result)->lineno;
        debug_end_offset = ((अभिव्यक्ति_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    अन्यथा {
        debug_end_पङ्क्ति = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    प्रतिज्ञा(exprstr != रिक्त);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_अभिव्यक्ति(exprstr)
        : Py_NewRef(exprstr);
    यदि (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        फर्काऊ रिक्त;
    }

    अभिव्यक्ति_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (अभिव्यक्ति_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    यदि (!interpolation || !debug) {
        फर्काऊ interpolation;
    }

    अभिव्यक्ति_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_पङ्क्ति,
                                            debug_end_offset - 1, p->arena);
    यदि (!debug_text) {
        फर्काऊ रिक्त;
    }

    asdl_अभिव्यक्ति_seq *मानहरू = Py_asdl_अभिव्यक्ति_seq_new(2, arena);
    यदि (!मानहरू) {
        फर्काऊ रिक्त;
    }
    asdl_seq_SET(मानहरू, 0, debug_text);
    asdl_seq_SET(मानहरू, 1, interpolation);
    फर्काऊ _PyAST_JoinedStr(मानहरू, lineno, col_offset, debug_end_पङ्क्ति, debug_end_offset, p->arena);
}

अभिव्यक्ति_ty Py_Pegen_formatted_मान(Parser *p, अभिव्यक्ति_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णाङ्क lineno, पूर्णाङ्क col_offset,
                                 पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset, PyArena *arena) {
    पूर्णाङ्क conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    अभिव्यक्ति_ty formatted_मान = Py_AST_Formatted_मान(
        expression, conversion_val, format ? (अभिव्यक्ति_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    यदि (!formatted_मान || !debug) {
        फर्काऊ formatted_मान;
    }

    /* Find the non whitespace token after the "=" */
    पूर्णाङ्क debug_end_पङ्क्ति, debug_end_offset;
    PyObject *debug_metadata;

    यदि (conversion) {
        debug_end_पङ्क्ति = ((अभिव्यक्ति_ty) conversion->result)->lineno;
        debug_end_offset = ((अभिव्यक्ति_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    अन्यथा यदि (format) {
        debug_end_पङ्क्ति = ((अभिव्यक्ति_ty) format->result)->lineno;
        debug_end_offset = ((अभिव्यक्ति_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    अन्यथा {
        debug_end_पङ्क्ति = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    अभिव्यक्ति_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_पङ्क्ति,
                                            debug_end_offset - 1, p->arena);
    यदि (!debug_text) {
        फर्काऊ रिक्त;
    }

    asdl_अभिव्यक्ति_seq *मानहरू = Py_asdl_अभिव्यक्ति_seq_new(2, arena);
    यदि (!मानहरू) {
        फर्काऊ रिक्त;
    }
    asdl_seq_SET(मानहरू, 0, debug_text);
    asdl_seq_SET(मानहरू, 1, formatted_मान);
    फर्काऊ _PyAST_JoinedStr(मानहरू, lineno, col_offset, debug_end_पङ्क्ति, debug_end_offset, p->arena);
}

स्थिर अभिव्यक्ति_ty
_build_concatenated_bytes(Parser *p, asdl_अभिव्यक्ति_seq *strings, पूर्णाङ्क lineno,
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
    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मान;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        यदि (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            फर्काऊ रिक्त;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    यदि (writer == रिक्त) {
        फर्काऊ रिक्त;
    }
    वर्ण *out = PyBytesWriter_GetData(writer);

    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मान;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        यदि (part > 0) {
            स_नकल(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    यदि (res == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

स्थिर अभिव्यक्ति_ty
_build_concatenated_unicode(Parser *p, asdl_अभिव्यक्ति_seq *strings, पूर्णाङ्क lineno,
                        पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 1);

    अभिव्यक्ति_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    यदि (writer == रिक्त) {
        फर्काऊ रिक्त;
    }

    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty current_elem = asdl_seq_GET(strings, i);
        प्रतिज्ञा(current_elem->kind == Constant_kind);

        यदि (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.मान)) {
            PyUnicodeWriter_Discard(writer);
            फर्काऊ रिक्त;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    यदि (final == रिक्त) {
        फर्काऊ रिक्त;
    }
    यदि (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

स्थिर asdl_अभिव्यक्ति_seq *
_build_concatenated_str(Parser *p, asdl_अभिव्यक्ति_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.मानहरू);
                रोक;
            पक्ष TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.मानहरू);
                रोक;
            स्वभाव:
                n_flattened_elements++;
                रोक;
        }
    }


    asdl_अभिव्यक्ति_seq* flattened = Py_asdl_अभिव्यक्ति_seq_new(n_flattened_elements, p->arena);
    यदि (flattened == रिक्त) {
        फर्काऊ रिक्त;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    लागि (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष JoinedStr_kind:
                लागि (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.मानहरू); j++) {
                    अभिव्यक्ति_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.मानहरू, j);
                    यदि (subvalue == रिक्त) {
                        फर्काऊ रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                रोक;
            पक्ष TemplateStr_kind:
                लागि (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.मानहरू); j++) {
                    अभिव्यक्ति_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.मानहरू, j);
                    यदि (subvalue == रिक्त) {
                        फर्काऊ रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                रोक;
            स्वभाव:
                asdl_seq_SET(flattened, current_pos++, elem);
                रोक;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    पूर्णाङ्क prev_is_constant = 0;
    लागि (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        यदि (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.मान) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.मान) == 0)
            जारी;

        यदि (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_अभिव्यक्ति_seq* मानहरू = Py_asdl_अभिव्यक्ति_seq_new(n_elements, p->arena);
    यदि (मानहरू == रिक्त) {
        फर्काऊ रिक्त;
    }

    /* build folded list */
    current_pos = 0;
    लागि (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        यदि (elem->kind == Constant_kind) {
            यदि (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                अभिव्यक्ति_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                यदि (writer == रिक्त) {
                    फर्काऊ रिक्त;
                }
                अभिव्यक्ति_ty last_elem = elem;
                Py_ssize_t j;
                लागि (j = i; j < n_flattened_elements; j++) {
                    अभिव्यक्ति_ty current_elem = asdl_seq_GET(flattened, j);
                    यदि (current_elem->kind == Constant_kind) {
                        यदि (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.मान)) {
                            PyUnicodeWriter_Discard(writer);
                            फर्काऊ रिक्त;
                        }
                        last_elem = current_elem;
                    } अन्यथा {
                        रोक;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                यदि (concat_str == रिक्त) {
                    फर्काऊ रिक्त;
                }
                यदि (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    फर्काऊ रिक्त;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                यदि (elem == रिक्त) {
                    फर्काऊ रिक्त;
                }
            }

            /* Drop all empty contanst strings */
            यदि (PyUnicode_CheckExact(elem->v.Constant.मान) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.मान) == 0) {
                जारी;
            }
        }

        asdl_seq_SET(मानहरू, current_pos++, elem);
    }

    प्रतिज्ञा(current_pos == n_elements);
    फर्काऊ मानहरू;
}

स्थिर अभिव्यक्ति_ty
_build_concatenated_joined_str(Parser *p, asdl_अभिव्यक्ति_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    asdl_अभिव्यक्ति_seq *मानहरू = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    यदि (!मानहरू) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_JoinedStr(मानहरू, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

अभिव्यक्ति_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_अभिव्यक्ति_seq *strings,
                               पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                               पूर्णाङ्क end_col_offset, PyArena *arena)
{
    asdl_अभिव्यक्ति_seq *मानहरू = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    यदि (!मानहरू) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyAST_TemplateStr(मानहरू, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

अभिव्यक्ति_ty
_PyPegen_concatenate_strings(Parser *p, asdl_अभिव्यक्ति_seq *strings,
                             पूर्णाङ्क lineno, पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno,
                             पूर्णाङ्क end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    पूर्णाङ्क f_string_found = 0;
    पूर्णाङ्क unicode_string_found = 0;
    पूर्णाङ्क bytes_found = 0;

    Py_ssize_t i = 0;
    लागि (i = 0; i < len; i++) {
        अभिव्यक्ति_ty elem = asdl_seq_GET(strings, i);
        विकल्प(elem->kind) {
            पक्ष Constant_kind:
                यदि (PyBytes_CheckExact(elem->v.Constant.मान)) {
                    bytes_found = 1;
                } अन्यथा {
                    unicode_string_found = 1;
                }
                रोक;
            पक्ष JoinedStr_kind:
                f_string_found = 1;
                रोक;
            पक्ष TemplateStr_kind:
                // python.gram handles this; we should never get here
                प्रतिज्ञा(0);
                रोक;
            स्वभाव:
                f_string_found = 1;
                रोक;
        }
    }

    // Cannot mix unicode and bytes
    यदि ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        फर्काऊ रिक्त;
    }

    // If it's only bytes or only unicode string, do a simple concat
    यदि (!f_string_found) {
        यदि (len == 1) {
            फर्काऊ asdl_seq_GET(strings, 0);
        }
        अन्यथा यदि (bytes_found) {
            फर्काऊ _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        अन्यथा {
            फर्काऊ _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    फर्काऊ _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

स्थिर पूर्णाङ्क
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, अभिव्यक्ति_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    यदि (p->call_invalid_rules) {
        फर्काऊ 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    यदि (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        फर्काऊ 0;
    }

    पूर्णाङ्क गन्ती = Py_Pegen_seq_गन्ती_dots(dots);
    वर्ण *buf = PyMem_RawMalloc(गन्ती + 1);
    यदि (buf == रिक्त) {
        PyErr_NoMemory();
        फर्काऊ -1;
    }
    स_रखो(buf, '.', गन्ती);
    buf[गन्ती] = '\0';

    PyObject *सन्देश = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    यदि (सन्देश == रिक्त) {
        फर्काऊ -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    पूर्णाङ्क res = _PyErr_EmitSyntaxWarning(सन्देश,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(सन्देश);
    फर्काऊ res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, अभिव्यक्ति_ty module_name,
                             asdl_alias_seq *names, अभिव्यक्ति_ty lazy_token, पूर्णाङ्क lineno,
                             पूर्णाङ्क col_offset, पूर्णाङ्क end_lineno, पूर्णाङ्क end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    पूर्णाङ्क level = Py_Pegen_seq_गन्ती_dots(dots);
    यदि (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        यदि (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            फर्काऊ रिक्त;
        }
        लागि (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
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
        यदि (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            फर्काऊ रिक्त;
        }
    }
    फर्काऊ _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    यदि (!p->call_invalid_rules) {
        फर्काऊ stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    यदि (len == 0) {
        फर्काऊ stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    यदि (p->last_stmt_location.lineno > last_stmt->lineno) {
        फर्काऊ stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    फर्काऊ stmts;
}
