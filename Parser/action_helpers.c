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
    परत &_PyRuntime.parser.dummy_name;
}

/* Creates a single-element asdl_seq* that contains a */
asdl_seq *
_PyPegen_singleton_seq(Parser *p, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    asdl_seq *seq = (asdl_seq*)_Py_asdl_generic_seq_new(1, p->arena);
    जर (!seq) {
        परत रिक्त;
    }
    asdl_seq_SET_UNTYPED(seq, 0, a);
    परत seq;
}

/* Creates a copy of seq and prepends a to it */
asdl_seq *
Py_Pegen_seq_घाला_in_front(Parser *p, शून्य *a, asdl_seq *seq)
{
    प्रतिज्ञा(a != रिक्त);
    जर (!seq) {
        परत _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }

    asdl_seq_SET_UNTYPED(new_seq, 0, a);
    साठी (Py_ssize_t i = 1, l = asdl_seq_LEN(new_seq); i < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i - 1));
    }
    परत new_seq;
}

/* Creates a copy of seq and appends a to it */
asdl_seq *
_PyPegen_seq_append_to_end(Parser *p, asdl_seq *seq, शून्य *a)
{
    प्रतिज्ञा(a != रिक्त);
    जर (!seq) {
        परत _PyPegen_singleton_seq(p, a);
    }

    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(asdl_seq_LEN(seq) + 1, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }

    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(new_seq); i + 1 < l; i++) {
        asdl_seq_SET_UNTYPED(new_seq, i, asdl_seq_GET_UNTYPED(seq, i));
    }
    asdl_seq_SET_UNTYPED(new_seq, asdl_seq_LEN(new_seq) - 1, a);
    परत new_seq;
}

स्थिर Py_ssize_t
_get_flattened_seq_size(asdl_seq *seqs)
{
    Py_ssize_t size = 0;
    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        size += asdl_seq_LEN(inner_seq);
    }
    परत size;
}

/* Flattens an asdl_seq* of asdl_seq*s */
asdl_seq *
_PyPegen_seq_flatten(Parser *p, asdl_seq *seqs)
{
    Py_ssize_t flattened_seq_size = _get_flattened_seq_size(seqs);
    प्रतिज्ञा(flattened_seq_size > 0);

    asdl_seq *flattened_seq = (asdl_seq*)_Py_asdl_generic_seq_new(flattened_seq_size, p->arena);
    जर (!flattened_seq) {
        परत रिक्त;
    }

    पूर्णांक flattened_seq_idx = 0;
    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(seqs); i < l; i++) {
        asdl_seq *inner_seq = asdl_seq_GET_UNTYPED(seqs, i);
        साठी (Py_ssize_t j = 0, li = asdl_seq_LEN(inner_seq); j < li; j++) {
            asdl_seq_SET_UNTYPED(flattened_seq, flattened_seq_idx++, asdl_seq_GET_UNTYPED(inner_seq, j));
        }
    }
    प्रतिज्ञा(flattened_seq_idx == flattened_seq_size);

    परत flattened_seq;
}

शून्य *
_PyPegen_seq_last_item(asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    परत asdl_seq_GET_UNTYPED(seq, len - 1);
}

शून्य *
_PyPegen_seq_first_item(asdl_seq *seq)
{
    परत asdl_seq_GET_UNTYPED(seq, 0);
}

/* Creates a new name of the form <first_name>.<second_name> */
अभिव्यक्ती_ty
_PyPegen_join_names_with_dot(Parser *p, अभिव्यक्ती_ty first_name, अभिव्यक्ती_ty second_name)
{
    प्रतिज्ञा(first_name != रिक्त && second_name != रिक्त);
    PyObject *uni = PyUnicode_FromFormat("%U.%U",
            first_name->v.Name.id, second_name->v.Name.id);
    जर (!uni) {
        परत रिक्त;
    }
    PyInterpreterState *interp = _PyInterpreterState_GET();
    _PyUnicode_InternImmortal(interp, &uni);
    जर (_PyArena_AddPyObject(p->arena, uni) < 0) {
        Py_DECREF(uni);
        परत रिक्त;
    }

    परत _PyAST_Name(uni, Load, EXTRA_अभिव्यक्ती(first_name, second_name));
}

/* Counts the total number of dots in seq's tokens */
पूर्णांक
Py_Pegen_seq_मोजणी_dots(asdl_seq *seq)
{
    पूर्णांक संख्या_of_dots = 0;
    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        Token *current_अभिव्यक्ती = asdl_seq_GET_UNTYPED(seq, i);
        निवड (current_अभिव्यक्ती->type) {
            स्थिती ELLIPSIS:
                संख्या_of_dots += 3;
                थांबा;
            स्थिती DOT:
                संख्या_of_dots += 1;
                थांबा;
            पूर्वनिर्धारित:
                Py_UNREACHABLE();
        }
    }

    परत संख्या_of_dots;
}

/* Creates an alias with '*' as the identifier name */
alias_ty
_PyPegen_alias_for_star(Parser *p, पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                        पूर्णांक end_col_offset, PyArena *arena) {
    PyObject *str = PyUnicode_InternFromString("*");
    जर (!str) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        परत रिक्त;
    }
    परत _PyAST_alias(str, रिक्त, lineno, col_offset, end_lineno, end_col_offset, arena);
}

/* Creates a new asdl_seq* with the identifiers of all the names in seq */
asdl_identifier_seq *
_PyPegen_map_names_to_ids(Parser *p, asdl_अभिव्यक्ती_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_identifier_seq *new_seq = _Py_asdl_identifier_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty e = asdl_seq_GET(seq, i);
        asdl_seq_SET(new_seq, i, e->v.Name.id);
    }
    परत new_seq;
}

/* Constructs a CmpopExprPair */
Cmpop_अभिव्यक्ती_Pair *
Py_Pegen_cmpop_अभिव्यक्ती_pair(Parser *p, cmpop_ty cmpop, अभिव्यक्ती_ty अभिव्यक्ती)
{
    प्रतिज्ञा(अभिव्यक्ती != रिक्त);
    Cmpop_अभिव्यक्ती_Pair *a = _PyArena_Malloc(p->arena, आकार(Cmpop_अभिव्यक्ती_Pair));
    जर (!a) {
        परत रिक्त;
    }
    a->cmpop = cmpop;
    a->अभिव्यक्ती = अभिव्यक्ती;
    परत a;
}

asdl_int_seq *
_PyPegen_get_cmpops(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_int_seq *new_seq = _Py_asdl_int_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_अभिव्यक्ती_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->cmpop);
    }
    परत new_seq;
}

asdl_अभिव्यक्ती_seq *
_PyPegen_get_exprs(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    प्रतिज्ञा(len > 0);

    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        Cmpop_अभिव्यक्ती_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->अभिव्यक्ती);
    }
    परत new_seq;
}

/* Creates an asdl_seq* where all the elements have been changed to have ctx as context */
स्थिर asdl_अभिव्यक्ती_seq *
_set_seq_context(Parser *p, asdl_अभिव्यक्ती_seq *seq, अभिव्यक्ती_context_ty ctx)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    जर (len == 0) {
        परत रिक्त;
    }

    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty e = asdl_seq_GET(seq, i);
        अभिव्यक्ती_ty new_e = Py_Pegen_set_अभिव्यक्ती_context(p, e, ctx);
        जर (!new_e) {
            परत रिक्त;
        }
        asdl_seq_SET(new_seq, i, new_e);
    }
    परत new_seq;
}

स्थिर अभिव्यक्ती_ty
_set_name_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    परत _PyAST_Name(e->v.Name.id, ctx, EXTRA_अभिव्यक्ती(e, e));
}

स्थिर अभिव्यक्ती_ty
_set_tuple_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    asdl_अभिव्यक्ती_seq *seq = _set_seq_context(p, e->v.Tuple.elts, ctx);
    जर (!seq && PyErr_Occurred()) {
        परत रिक्त;
    }
    परत _PyAST_Tuple(seq, ctx, EXTRA_अभिव्यक्ती(e, e));
}

स्थिर अभिव्यक्ती_ty
_set_list_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    asdl_अभिव्यक्ती_seq *seq = _set_seq_context(p, e->v.List.elts, ctx);
    जर (!seq && PyErr_Occurred()) {
        परत रिक्त;
    }
    परत _PyAST_List(seq, ctx, EXTRA_अभिव्यक्ती(e, e));
}

स्थिर अभिव्यक्ती_ty
_set_subscript_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    परत _PyAST_Subscript(e->v.Subscript.मूल्य, e->v.Subscript.slice,
                            ctx, EXTRA_अभिव्यक्ती(e, e));
}

स्थिर अभिव्यक्ती_ty
_set_attribute_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    परत _PyAST_Attribute(e->v.Attribute.मूल्य, e->v.Attribute.attr,
                            ctx, EXTRA_अभिव्यक्ती(e, e));
}

स्थिर अभिव्यक्ती_ty
_set_starred_context(Parser *p, अभिव्यक्ती_ty e, अभिव्यक्ती_context_ty ctx)
{
    अभिव्यक्ती_ty inner = Py_Pegen_set_अभिव्यक्ती_context(p, e->v.Starred.मूल्य, ctx);
    जर (!inner) {
        परत रिक्त;
    }
    परत _PyAST_Starred(inner, ctx, EXTRA_अभिव्यक्ती(e, e));
}

/* Creates an `expr_ty` equivalent to `expr` but with `ctx` as context */
अभिव्यक्ती_ty
Py_Pegen_set_अभिव्यक्ती_context(Parser *p, अभिव्यक्ती_ty अभिव्यक्ती, अभिव्यक्ती_context_ty ctx)
{
    प्रतिज्ञा(अभिव्यक्ती != रिक्त);

    अभिव्यक्ती_ty new = रिक्त;
    निवड (अभिव्यक्ती->kind) {
        स्थिती Name_kind:
            new = _set_name_context(p, अभिव्यक्ती, ctx);
            थांबा;
        स्थिती Tuple_kind:
            new = _set_tuple_context(p, अभिव्यक्ती, ctx);
            थांबा;
        स्थिती List_kind:
            new = _set_list_context(p, अभिव्यक्ती, ctx);
            थांबा;
        स्थिती Subscript_kind:
            new = _set_subscript_context(p, अभिव्यक्ती, ctx);
            थांबा;
        स्थिती Attribute_kind:
            new = _set_attribute_context(p, अभिव्यक्ती, ctx);
            थांबा;
        स्थिती Starred_kind:
            new = _set_starred_context(p, अभिव्यक्ती, ctx);
            थांबा;
        पूर्वनिर्धारित:
            new = अभिव्यक्ती;
    }
    परत new;
}

/* Invalid targets return NULL without raising, allowing parsing to backtrack.
   Copy valid targets to preserve the memoized expression's context. */
अभिव्यक्ती_ty
_PyPegen_make_target(Parser *p, अभिव्यक्ती_ty अभिव्यक्ती, TARGETS_TYPE targets_type)
{
    प्रतिज्ञा(अभिव्यक्ती != रिक्त);
    प्रतिज्ञा(targets_type != FOR_TARGETS);
    जर (_PyPegen_get_invalid_target(अभिव्यक्ती, targets_type) != रिक्त) {
        परत रिक्त;
    }
    परत Py_Pegen_set_अभिव्यक्ती_context(
        p, अभिव्यक्ती, targets_type == DEL_TARGETS ? Del : Store);
}

/* Constructs a KeyValuePair that is used when parsing a dict's key value pairs */
किल्ली_मूल्य_Pair *
Py_Pegen_किल्ली_मूल्य_pair(Parser *p, अभिव्यक्ती_ty किल्ली, अभिव्यक्ती_ty मूल्य)
{
    किल्ली_मूल्य_Pair *a = _PyArena_Malloc(p->arena, आकार(किल्ली_मूल्य_Pair));
    जर (!a) {
        परत रिक्त;
    }
    a->किल्ली = किल्ली;
    a->मूल्य = मूल्य;
    परत a;
}

/* Extracts all keys from an asdl_seq* of KeyValuePair*'s */
asdl_अभिव्यक्ती_seq *
Py_Pegen_get_किल्ल्या(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        किल्ली_मूल्य_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->किल्ली);
    }
    परत new_seq;
}

/* Extracts all values from an asdl_seq* of KeyValuePair*'s */
asdl_अभिव्यक्ती_seq *
Py_Pegen_get_मूल्ये(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        किल्ली_मूल्य_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->मूल्य);
    }
    परत new_seq;
}

/* Constructs a KeyPatternPair that is used when parsing mapping & class patterns */
किल्ली_Pattern_Pair *
Py_Pegen_किल्ली_pattern_pair(Parser *p, अभिव्यक्ती_ty किल्ली, pattern_ty pattern)
{
    किल्ली_Pattern_Pair *a = _PyArena_Malloc(p->arena, आकार(किल्ली_Pattern_Pair));
    जर (!a) {
        परत रिक्त;
    }
    a->किल्ली = किल्ली;
    a->pattern = pattern;
    परत a;
}

/* Extracts all keys from an asdl_seq* of KeyPatternPair*'s */
asdl_अभिव्यक्ती_seq *
Py_Pegen_get_pattern_किल्ल्या(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        किल्ली_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->किल्ली);
    }
    परत new_seq;
}

/* Extracts all patterns from an asdl_seq* of KeyPatternPair*'s */
asdl_pattern_seq *
_PyPegen_get_patterns(Parser *p, asdl_seq *seq)
{
    Py_ssize_t len = asdl_seq_LEN(seq);
    asdl_pattern_seq *new_seq = _Py_asdl_pattern_seq_new(len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        किल्ली_Pattern_Pair *pair = asdl_seq_GET_UNTYPED(seq, i);
        asdl_seq_SET(new_seq, i, pair->pattern);
    }
    परत new_seq;
}

/* Constructs a NameDefaultPair */
NameDefaultPair *
_PyPegen_name_default_pair(Parser *p, arg_ty arg, अभिव्यक्ती_ty मूल्य, Token *tc)
{
    NameDefaultPair *a = _PyArena_Malloc(p->arena, आकार(NameDefaultPair));
    जर (!a) {
        परत रिक्त;
    }
    a->arg = _PyPegen_add_type_comment_to_arg(p, arg, tc);
    जर (!a->arg) {
        परत रिक्त;
    }
    a->मूल्य = मूल्य;
    परत a;
}

/* Constructs a SlashWithDefault */
SlashWithDefault *
_PyPegen_slash_with_default(Parser *p, asdl_arg_seq *plain_names, asdl_seq *names_with_defaults)
{
    SlashWithDefault *a = _PyArena_Malloc(p->arena, आकार(SlashWithDefault));
    जर (!a) {
        परत रिक्त;
    }
    a->plain_names = plain_names;
    a->names_with_defaults = names_with_defaults;
    परत a;
}

/* Constructs a StarEtc */
StarEtc *
_PyPegen_star_etc(Parser *p, arg_ty vararg, asdl_seq *kwonlyargs, arg_ty kwarg)
{
    StarEtc *a = _PyArena_Malloc(p->arena, आकार(StarEtc));
    जर (!a) {
        परत रिक्त;
    }
    a->vararg = vararg;
    a->kwonlyargs = kwonlyargs;
    a->kwarg = kwarg;
    परत a;
}

asdl_seq *
_PyPegen_join_sequences(Parser *p, asdl_seq *a, asdl_seq *b)
{
    Py_ssize_t first_len = asdl_seq_LEN(a);
    Py_ssize_t second_len = asdl_seq_LEN(b);
    asdl_seq *new_seq = (asdl_seq*)_Py_asdl_generic_seq_new(first_len + second_len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }

    पूर्णांक k = 0;
    साठी (Py_ssize_t i = 0; i < first_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(a, i));
    }
    साठी (Py_ssize_t i = 0; i < second_len; i++) {
        asdl_seq_SET_UNTYPED(new_seq, k++, asdl_seq_GET_UNTYPED(b, i));
    }

    परत new_seq;
}

स्थिर asdl_arg_seq*
_get_names(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_arg_seq *seq = _Py_asdl_arg_seq_new(len, p->arena);
    जर (!seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->arg);
    }
    परत seq;
}

स्थिर asdl_अभिव्यक्ती_seq *
_get_defaults(Parser *p, asdl_seq *names_with_defaults)
{
    Py_ssize_t len = asdl_seq_LEN(names_with_defaults);
    asdl_अभिव्यक्ती_seq *seq = Py_asdl_अभिव्यक्ती_seq_new(len, p->arena);
    जर (!seq) {
        परत रिक्त;
    }
    साठी (Py_ssize_t i = 0; i < len; i++) {
        NameDefaultPair *pair = asdl_seq_GET_UNTYPED(names_with_defaults, i);
        asdl_seq_SET(seq, i, pair->मूल्य);
    }
    परत seq;
}

स्थिर पूर्णांक
_make_posonlyargs(Parser *p,
                  asdl_arg_seq *slash_without_default,
                  SlashWithDefault *slash_with_default,
                  asdl_arg_seq **posonlyargs) {
    जर (slash_without_default != रिक्त) {
        *posonlyargs = slash_without_default;
    }
    अन्यथा जर (slash_with_default != रिक्त) {
        asdl_arg_seq *slash_with_default_names =
                _get_names(p, slash_with_default->names_with_defaults);
        जर (!slash_with_default_names) {
            परत -1;
        }
        *posonlyargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default->plain_names,
                (asdl_seq*)slash_with_default_names);
    }
    अन्यथा {
        *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }
    परत *posonlyargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णांक
_make_posargs(Parser *p,
              asdl_arg_seq *plain_names,
              asdl_seq *names_with_default,
              asdl_arg_seq **posargs) {

    जर (names_with_default != रिक्त) {
        जर (plain_names != रिक्त) {
            asdl_arg_seq *names_with_default_names = _get_names(p, names_with_default);
            जर (!names_with_default_names) {
                परत -1;
            }
            *posargs = (asdl_arg_seq*)_PyPegen_join_sequences(
                    p,(asdl_seq*)plain_names, (asdl_seq*)names_with_default_names);
        }
        अन्यथा {
            *posargs = _get_names(p, names_with_default);
        }
    }
    अन्यथा {
        जर (plain_names != रिक्त) {
            // With the current grammar, we never get here.
            // If that has changed, remove the assert, and test thoroughly.
            प्रतिज्ञा(0);
            *posargs = plain_names;
        }
        अन्यथा {
            *posargs = _Py_asdl_arg_seq_new(0, p->arena);
        }
    }
    परत *posargs == रिक्त ? -1 : 0;
}

स्थिर पूर्णांक
_make_posdefaults(Parser *p,
                  SlashWithDefault *slash_with_default,
                  asdl_seq *names_with_default,
                  asdl_अभिव्यक्ती_seq **posdefaults) {
    जर (slash_with_default != रिक्त && names_with_default != रिक्त) {
        asdl_अभिव्यक्ती_seq *slash_with_default_मूल्ये =
                _get_defaults(p, slash_with_default->names_with_defaults);
        जर (!slash_with_default_मूल्ये) {
            परत -1;
        }
        asdl_अभिव्यक्ती_seq *names_with_default_मूल्ये = _get_defaults(p, names_with_default);
        जर (!names_with_default_मूल्ये) {
            परत -1;
        }
        *posdefaults = (asdl_अभिव्यक्ती_seq*)_PyPegen_join_sequences(
                p,
                (asdl_seq*)slash_with_default_मूल्ये,
                (asdl_seq*)names_with_default_मूल्ये);
    }
    अन्यथा जर (slash_with_default == रिक्त && names_with_default != रिक्त) {
        *posdefaults = _get_defaults(p, names_with_default);
    }
    अन्यथा जर (slash_with_default != रिक्त && names_with_default == रिक्त) {
        *posdefaults = _get_defaults(p, slash_with_default->names_with_defaults);
    }
    अन्यथा {
        *posdefaults = Py_asdl_अभिव्यक्ती_seq_new(0, p->arena);
    }
    परत *posdefaults == रिक्त ? -1 : 0;
}

स्थिर पूर्णांक
_make_kwargs(Parser *p, StarEtc *star_etc,
             asdl_arg_seq **kwonlyargs,
             asdl_अभिव्यक्ती_seq **kwdefaults) {
    जर (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwonlyargs = _get_names(p, star_etc->kwonlyargs);
    }
    अन्यथा {
        *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    }

    जर (*kwonlyargs == रिक्त) {
        परत -1;
    }

    जर (star_etc != रिक्त && star_etc->kwonlyargs != रिक्त) {
        *kwdefaults = _get_defaults(p, star_etc->kwonlyargs);
    }
    अन्यथा {
        *kwdefaults = Py_asdl_अभिव्यक्ती_seq_new(0, p->arena);
    }

    जर (*kwdefaults == रिक्त) {
        परत -1;
    }

    परत 0;
}

/* Constructs an arguments_ty object out of all the parsed constructs in the parameters rule */
arguments_ty
_PyPegen_make_arguments(Parser *p, asdl_arg_seq *slash_without_default,
                        SlashWithDefault *slash_with_default, asdl_arg_seq *plain_names,
                        asdl_seq *names_with_default, StarEtc *star_etc)
{
    asdl_arg_seq *posonlyargs;
    जर (_make_posonlyargs(p, slash_without_default, slash_with_default, &posonlyargs) == -1) {
        परत रिक्त;
    }

    asdl_arg_seq *posargs;
    जर (_make_posargs(p, plain_names, names_with_default, &posargs) == -1) {
        परत रिक्त;
    }

    asdl_अभिव्यक्ती_seq *posdefaults;
    जर (_make_posdefaults(p,slash_with_default, names_with_default, &posdefaults) == -1) {
        परत रिक्त;
    }

    arg_ty vararg = रिक्त;
    जर (star_etc != रिक्त && star_etc->vararg != रिक्त) {
        vararg = star_etc->vararg;
    }

    asdl_arg_seq *kwonlyargs;
    asdl_अभिव्यक्ती_seq *kwdefaults;
    जर (_make_kwargs(p, star_etc, &kwonlyargs, &kwdefaults) == -1) {
        परत रिक्त;
    }

    arg_ty kwarg = रिक्त;
    जर (star_etc != रिक्त && star_etc->kwarg != रिक्त) {
        kwarg = star_etc->kwarg;
    }

    परत _PyAST_arguments(posonlyargs, posargs, vararg, kwonlyargs,
                            kwdefaults, kwarg, posdefaults, p->arena);
}


/* Constructs an empty arguments_ty object, that gets used when a function accepts no
 * arguments. */
arguments_ty
_PyPegen_empty_arguments(Parser *p)
{
    asdl_arg_seq *posonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    जर (!posonlyargs) {
        परत रिक्त;
    }
    asdl_arg_seq *posargs = _Py_asdl_arg_seq_new(0, p->arena);
    जर (!posargs) {
        परत रिक्त;
    }
    asdl_अभिव्यक्ती_seq *posdefaults = Py_asdl_अभिव्यक्ती_seq_new(0, p->arena);
    जर (!posdefaults) {
        परत रिक्त;
    }
    asdl_arg_seq *kwonlyargs = _Py_asdl_arg_seq_new(0, p->arena);
    जर (!kwonlyargs) {
        परत रिक्त;
    }
    asdl_अभिव्यक्ती_seq *kwdefaults = Py_asdl_अभिव्यक्ती_seq_new(0, p->arena);
    जर (!kwdefaults) {
        परत रिक्त;
    }

    परत _PyAST_arguments(posonlyargs, posargs, रिक्त, kwonlyargs,
                            kwdefaults, रिक्त, posdefaults, p->arena);
}

/* Encapsulates the value of an operator_ty into an AugOperator struct */
AugOperator *
_PyPegen_augoperator(Parser *p, operator_ty kind)
{
    AugOperator *a = _PyArena_Malloc(p->arena, आकार(AugOperator));
    जर (!a) {
        परत रिक्त;
    }
    a->kind = kind;
    परत a;
}

/* Construct a FunctionDef equivalent to function_def, but with decorators */
stmt_ty
_PyPegen_function_def_decorators(Parser *p, asdl_अभिव्यक्ती_seq *decorators, stmt_ty function_def)
{
    प्रतिज्ञा(function_def != रिक्त);
    जर (function_def->kind == AsyncFunctionDef_kind) {
        परत _PyAST_AsyncFunctionDef(
            function_def->v.AsyncFunctionDef.name,
            function_def->v.AsyncFunctionDef.args,
            function_def->v.AsyncFunctionDef.body, decorators,
            function_def->v.AsyncFunctionDef.returns,
            function_def->v.AsyncFunctionDef.type_comment,
            function_def->v.AsyncFunctionDef.type_params,
            function_def->lineno, function_def->col_offset,
            function_def->end_lineno, function_def->end_col_offset, p->arena);
    }

    परत _PyAST_FunctionDef(
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
_PyPegen_class_def_decorators(Parser *p, asdl_अभिव्यक्ती_seq *decorators, stmt_ty class_def)
{
    प्रतिज्ञा(class_def != रिक्त);
    परत _PyAST_ClassDef(
        class_def->v.ClassDef.name,
        class_def->v.ClassDef.bases, class_def->v.ClassDef.keywords,
        class_def->v.ClassDef.body, decorators,
        class_def->v.ClassDef.type_params,
        class_def->lineno, class_def->col_offset, class_def->end_lineno,
        class_def->end_col_offset, p->arena);
}

/* Construct a KeywordOrStarred */
KeywordOrStarred *
_PyPegen_keyword_or_starred(Parser *p, शून्य *element, पूर्णांक is_keyword)
{
    KeywordOrStarred *a = _PyArena_Malloc(p->arena, आकार(KeywordOrStarred));
    जर (!a) {
        परत रिक्त;
    }
    a->element = element;
    a->is_keyword = is_keyword;
    परत a;
}

/* Get the number of starred expressions in an asdl_seq* of KeywordOrStarred*s */
स्थिर पूर्णांक
seq_संख्या_of_starred_exprs(asdl_seq *seq)
{
    पूर्णांक n = 0;
    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(seq); i < l; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(seq, i);
        जर (!k->is_keyword) {
            n++;
        }
    }
    परत n;
}

/* Extract the starred expressions of an asdl_seq* of KeywordOrStarred*s */
asdl_अभिव्यक्ती_seq *
_PyPegen_seq_extract_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    पूर्णांक new_len = seq_संख्या_of_starred_exprs(kwargs);
    जर (new_len == 0) {
        परत रिक्त;
    }
    asdl_अभिव्यक्ती_seq *new_seq = Py_asdl_अभिव्यक्ती_seq_new(new_len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }

    पूर्णांक idx = 0;
    साठी (Py_ssize_t i = 0, len = asdl_seq_LEN(kwargs); i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        जर (!k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    परत new_seq;
}

/* Return a new asdl_seq* with only the keywords in kwargs */
asdl_keyword_seq*
_PyPegen_seq_delete_starred_exprs(Parser *p, asdl_seq *kwargs)
{
    Py_ssize_t len = asdl_seq_LEN(kwargs);
    Py_ssize_t new_len = len - seq_संख्या_of_starred_exprs(kwargs);
    जर (new_len == 0) {
        परत रिक्त;
    }
    asdl_keyword_seq *new_seq = _Py_asdl_keyword_seq_new(new_len, p->arena);
    जर (!new_seq) {
        परत रिक्त;
    }

    पूर्णांक idx = 0;
    साठी (Py_ssize_t i = 0; i < len; i++) {
        KeywordOrStarred *k = asdl_seq_GET_UNTYPED(kwargs, i);
        जर (k->is_keyword) {
            asdl_seq_SET(new_seq, idx++, k->element);
        }
    }
    परत new_seq;
}

अभिव्यक्ती_ty
_PyPegen_ensure_imaginary(Parser *p, अभिव्यक्ती_ty exp)
{
    जर (exp->kind != Constant_kind || !PyComplex_CheckExact(exp->v.Constant.मूल्य)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "imaginary number required in complex literal");
        परत रिक्त;
    }
    परत exp;
}

अभिव्यक्ती_ty
_PyPegen_ensure_real(Parser *p, अभिव्यक्ती_ty exp)
{
    जर (exp->kind != Constant_kind || PyComplex_CheckExact(exp->v.Constant.मूल्य)) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(exp, "real number required in complex literal");
        परत रिक्त;
    }
    परत exp;
}

mod_ty
_PyPegen_make_module(Parser *p, asdl_stmt_seq *a) {
    asdl_type_ignore_seq *type_ignores = रिक्त;
    Py_ssize_t अंक = p->type_ignore_comments.अंक_वस्तुसंच;
    जर (अंक > 0) {
        // Turn the raw (comment, lineno) pairs into TypeIgnore objects in the arena
        type_ignores = _Py_asdl_type_ignore_seq_new(अंक, p->arena);
        जर (type_ignores == रिक्त) {
            परत रिक्त;
        }
        साठी (Py_ssize_t i = 0; i < अंक; i++) {
            PyObject *tag = _PyPegen_new_type_comment(p, p->type_ignore_comments.वस्तुसंच[i].comment);
            जर (tag == रिक्त) {
                परत रिक्त;
            }
            type_ignore_ty ti = _PyAST_TypeIgnore(p->type_ignore_comments.वस्तुसंच[i].lineno,
                                                  tag, p->arena);
            जर (ti == रिक्त) {
                परत रिक्त;
            }
            asdl_seq_SET(type_ignores, i, ti);
        }
    }
    परत _PyAST_Module(a, type_ignores, p->arena);
}

PyObject *
_PyPegen_new_type_comment(Parser *p, स्थिरांक अक्षर *s)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, म_माप(s), रिक्त);
    जर (res == रिक्त) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, res) < 0) {
        Py_DECREF(res);
        परत रिक्त;
    }
    परत res;
}

arg_ty
_PyPegen_add_type_comment_to_arg(Parser *p, arg_ty a, Token *tc)
{
    जर (tc == रिक्त) {
        परत a;
    }
    स्थिरांक अक्षर *bytes = PyBytes_AsString(tc->bytes);
    जर (bytes == रिक्त) {
        परत रिक्त;
    }
    PyObject *tco = _PyPegen_new_type_comment(p, bytes);
    जर (tco == रिक्त) {
        परत रिक्त;
    }
    परत _PyAST_arg(a->arg, a->annotation, tco,
                      a->lineno, a->col_offset, a->end_lineno, a->end_col_offset,
                      p->arena);
}

/* Checks if the NOTEQUAL token is valid given the current parser flags
0 indicates success and nonzero indicates failure (an exception may be set) */
पूर्णांक
_PyPegen_check_barry_as_flufl(Parser *p, Token* t) {
    प्रतिज्ञा(t->bytes != रिक्त);
    प्रतिज्ञा(t->type == NOTEQUAL);

    स्थिरांक अक्षर* tok_str = PyBytes_AS_STRING(t->bytes);
    जर (p->flags & PyPARSE_BARRY_AS_BDFL && म_भेद(tok_str, "<>") != 0) {
        RAISE_SYNTAX_ERROR("with Barry as BDFL, use '<>' instead of '!='");
        परत -1;
    }
    जर (!(p->flags & PyPARSE_BARRY_AS_BDFL)) {
        परत म_भेद(tok_str, "!=");
    }
    परत 0;
}

पूर्णांक
_PyPegen_check_legacy_stmt(Parser *p, अभिव्यक्ती_ty name) {
    जर (name->kind != Name_kind) {
        परत 0;
    }
    स्थिरांक अक्षर* candidates[2] = {"print", "exec"};
    साठी (पूर्णांक i=0; i<2; i++) {
        जर (PyUnicode_CompareWithASCIIString(name->v.Name.id, candidates[i]) == 0) {
            परत 1;
        }
    }
    परत 0;
}

शून्य *
_PyPegen_raise_error_for_missing_comma(Parser *p, अभिव्यक्ती_ty a, अभिव्यक्ती_ty b)
{
    // Don't raise for legacy statements like "print x" or "exec x"
    जर (_PyPegen_check_legacy_stmt(p, a)) {
        परत रिक्त;
    }
    // Only raise inside parentheses/brackets (level > 0)
    जर (p->tokens[p->mark - 1]->level == 0) {
        परत रिक्त;
    }
    // For multi-line expressions (like string concatenations), point to the
    // last line instead of the first for a more helpful error message.
    // Use a->col_offset as the starting column since all strings in the
    // concatenation typically share the same indentation.
    जर (a->end_lineno > a->lineno) {
        परत RAISE_ERROR_KNOWN_LOCATION(
            p, PyExc_SyntaxError, a->end_lineno, a->col_offset,
            a->end_lineno, a->end_col_offset,
            "invalid syntax. Perhaps you forgot a comma?"
        );
    }
    परत RAISE_ERROR_KNOWN_LOCATION(
        p, PyExc_SyntaxError, a->lineno, a->col_offset,
        b->end_lineno, b->end_col_offset,
        "invalid syntax. Perhaps you forgot a comma?"
    );
}

स्थिर ResultTokenWithMetadata *
result_token_with_metadata(Parser *p, शून्य *result, PyObject *metadata)
{
    ResultTokenWithMetadata *res = _PyArena_Malloc(p->arena, आकार(ResultTokenWithMetadata));
    जर (res == रिक्त) {
        परत रिक्त;
    }
    res->metadata = metadata;
    res->result = result;
    परत res;
}

स्थिर अक्षर
formatted_string_prefix(स्थिरांक Parser *p)
{
    पूर्णांक nested = 0;
    साठी (पूर्णांक i = p->mark - 1; i >= 0; i--) {
        पूर्णांक type = p->tokens[i]->type;
        जर (type == FSTRING_END || type == TSTRING_END) {
            nested++;
        }
        अन्यथा जर (type == FSTRING_START || type == TSTRING_START) {
            जर (nested == 0) {
                परत type == TSTRING_START ? 't' : 'f';
            }
            nested--;
        }
    }
    Py_UNREACHABLE();
}

ResultTokenWithMetadata *
_PyPegen_check_fstring_conversion(Parser *p, Token* conv_token, अभिव्यक्ती_ty conv)
{
    जर (conv_token->lineno != conv->lineno || conv_token->end_col_offset != conv->col_offset) {
        परत RAISE_SYNTAX_ERROR_KNOWN_RANGE(
            conv_token, conv,
            "%c-string: conversion type must come right after the exclamation mark",
            formatted_string_prefix(p)
        );
    }

    Py_UCS4 first = PyUnicode_READ_CHAR(conv->v.Name.id, 0);
    जर (PyUnicode_GET_LENGTH(conv->v.Name.id) > 1 ||
            !(first == 's' || first == 'r' || first == 'a')) {
        RAISE_SYNTAX_ERROR_KNOWN_LOCATION(conv,
                                            "%c-string: invalid conversion character %R: expected 's', 'r', or 'a'",
                                            formatted_string_prefix(p),
                                            conv->v.Name.id);
        परत रिक्त;
    }

    परत result_token_with_metadata(p, conv, conv_token->metadata);
}

ResultTokenWithMetadata *
_PyPegen_setup_full_format_spec(Parser *p, Token *colon, asdl_अभिव्यक्ती_seq *spec, पूर्णांक lineno, पूर्णांक col_offset,
                                पूर्णांक end_lineno, पूर्णांक end_col_offset, PyArena *arena)
{
    जर (!spec) {
        परत रिक्त;
    }

    // This is needed to keep compatibility with 3.11, where an empty format
    // spec is parsed as an *empty* JoinedStr node, instead of having an empty
    // constant in it.
    Py_ssize_t n_वस्तुसंच = asdl_seq_LEN(spec);
    Py_ssize_t non_empty_मोजणी = 0;
    साठी (Py_ssize_t i = 0; i < n_वस्तुसंच; i++) {
        अभिव्यक्ती_ty item = asdl_seq_GET(spec, i);
        non_empty_मोजणी += !(item->kind == Constant_kind &&
                             PyUnicode_CheckExact(item->v.Constant.मूल्य) &&
                             PyUnicode_GET_LENGTH(item->v.Constant.मूल्य) == 0);
    }
    जर (non_empty_मोजणी != n_वस्तुसंच) {
        asdl_अभिव्यक्ती_seq *resized_spec =
            Py_asdl_अभिव्यक्ती_seq_new(non_empty_मोजणी, p->arena);
        जर (resized_spec == रिक्त) {
            परत रिक्त;
        }
        Py_ssize_t j = 0;
        साठी (Py_ssize_t i = 0; i < n_वस्तुसंच; i++) {
            अभिव्यक्ती_ty item = asdl_seq_GET(spec, i);
            जर (item->kind == Constant_kind &&
                PyUnicode_CheckExact(item->v.Constant.मूल्य) &&
                PyUnicode_GET_LENGTH(item->v.Constant.मूल्य) == 0) {
                सुरूठेवा;
            }
            asdl_seq_SET(resized_spec, j++, item);
        }
        प्रतिज्ञा(j == non_empty_मोजणी);
        spec = resized_spec;
    }
    अभिव्यक्ती_ty res;
    Py_ssize_t n = asdl_seq_LEN(spec);
    जर (n == 0 || (n == 1 && asdl_seq_GET(spec, 0)->kind == Constant_kind)) {
        res = _PyAST_JoinedStr(spec, lineno, col_offset, end_lineno,
                                    end_col_offset, p->arena);
    } अन्यथा {
        res = _PyPegen_concatenate_strings(p, spec,
                             lineno, col_offset, end_lineno,
                             end_col_offset, arena);
    }
    जर (!res) {
        परत रिक्त;
    }
    परत result_token_with_metadata(p, res, colon->metadata);
}

स्थिरांक अक्षर *
Py_Pegen_get_अभिव्यक्ती_name(अभिव्यक्ती_ty e)
{
    प्रतिज्ञा(e != रिक्त);
    निवड (e->kind) {
        स्थिती Attribute_kind:
            परत "attribute";
        स्थिती Subscript_kind:
            परत "subscript";
        स्थिती Starred_kind:
            परत "starred";
        स्थिती Name_kind:
            परत "name";
        स्थिती List_kind:
            परत "list";
        स्थिती Tuple_kind:
            परत "tuple";
        स्थिती Lambda_kind:
            परत "lambda";
        स्थिती Call_kind:
            परत "function call";
        स्थिती BoolOp_kind:
        स्थिती BinOp_kind:
        स्थिती UnaryOp_kind:
            परत "expression";
        स्थिती GeneratorExp_kind:
            परत "generator expression";
        स्थिती Yield_kind:
        स्थिती YieldFrom_kind:
            परत "yield expression";
        स्थिती Await_kind:
            परत "await expression";
        स्थिती ListComp_kind:
            परत "list comprehension";
        स्थिती SetComp_kind:
            परत "set comprehension";
        स्थिती DictComp_kind:
            परत "dict comprehension";
        स्थिती Dict_kind:
            परत "dict literal";
        स्थिती Set_kind:
            परत "set display";
        स्थिती JoinedStr_kind:
        स्थिती Formatted_मूल्य_kind:
            परत "f-string expression";
        स्थिती TemplateStr_kind:
        स्थिती Interpolation_kind:
            परत "t-string expression";
        स्थिती Constant_kind: {
            PyObject *मूल्य = e->v.Constant.मूल्य;
            जर (मूल्य == Py_None) {
                परत "None";
            }
            जर (मूल्य == Py_False) {
                परत "False";
            }
            जर (मूल्य == Py_True) {
                परत "True";
            }
            जर (मूल्य == Py_Ellipsis) {
                परत "ellipsis";
            }
            परत "literal";
        }
        स्थिती Compare_kind:
            परत "comparison";
        स्थिती IfExp_kind:
            परत "conditional expression";
        स्थिती Named_अभिव्यक्ती_kind:
            परत "named expression";
        पूर्वनिर्धारित:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            परत रिक्त;
    }
}

अभिव्यक्ती_ty
_PyPegen_get_last_comprehension_item(comprehension_ty comprehension) {
    जर (comprehension->ifs == रिक्त || asdl_seq_LEN(comprehension->ifs) == 0) {
        परत comprehension->iter;
    }
    परत PyPegen_last_item(comprehension->ifs, अभिव्यक्ती_ty);
}

अभिव्यक्ती_ty _PyPegen_collect_call_seqs(Parser *p, asdl_अभिव्यक्ती_seq *a, asdl_seq *b,
                     पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                     पूर्णांक end_col_offset, PyArena *arena) {
    Py_ssize_t args_len = asdl_seq_LEN(a);
    Py_ssize_t total_len = args_len;

    जर (b == रिक्त) {
        परत _PyAST_Call(_PyPegen_dummy_name(p), a, रिक्त, lineno, col_offset,
                        end_lineno, end_col_offset, arena);

    }

    asdl_अभिव्यक्ती_seq *starreds = _PyPegen_seq_extract_starred_exprs(p, b);
    जर (!starreds && PyErr_Occurred()) {
        परत रिक्त;
    }

    asdl_keyword_seq *keywords = _PyPegen_seq_delete_starred_exprs(p, b);
    जर (!keywords && PyErr_Occurred()) {
        परत रिक्त;
    }

    जर (starreds) {
        total_len += asdl_seq_LEN(starreds);
    }

    asdl_अभिव्यक्ती_seq *args = Py_asdl_अभिव्यक्ती_seq_new(total_len, arena);
    जर (args == रिक्त) {
        परत रिक्त;
    }

    Py_ssize_t i = 0;
    साठी (i = 0; i < args_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(a, i));
    }
    साठी (; i < total_len; i++) {
        asdl_seq_SET(args, i, asdl_seq_GET(starreds, i - args_len));
    }

    परत _PyAST_Call(_PyPegen_dummy_name(p), args, keywords, lineno,
                       col_offset, end_lineno, end_col_offset, arena);
}

// AST Error reporting helpers

अभिव्यक्ती_ty
_PyPegen_get_invalid_target(अभिव्यक्ती_ty e, TARGETS_TYPE targets_type)
{
    जर (e == रिक्त) {
        परत रिक्त;
    }

    जर (targets_type == SINGLE_TARGETS ||
        targets_type == ATTRIBUTE_OR_SUBSCRIPT_TARGETS) {
        जर (targets_type == SINGLE_TARGETS && e->kind == Name_kind) {
            परत रिक्त;
        }
        परत e->kind == Attribute_kind || e->kind == Subscript_kind ? रिक्त : e;
    }

#define VISIT_CONTAINER(CONTAINER, TYPE) करा { \
        Py_ssize_t len = asdl_seq_LEN((CONTAINER)->v.TYPE.elts);\
        साठी (Py_ssize_t i = 0; i < len; i++) {\
            अभिव्यक्ती_ty other = asdl_seq_GET((CONTAINER)->v.TYPE.elts, i);\
            अभिव्यक्ती_ty child = _PyPegen_get_invalid_target(other, targets_type);\
            जर (child != रिक्त) {\
                परत child;\
            }\
        }\
    } पर्यंत (0)

    // We only need to visit List and Tuple nodes recursively as those
    // are the only ones that can contain valid names in targets when
    // they are parsed as expressions. Any other kind of expression
    // that is a container (like Sets or Dicts) is directly invalid and
    // we don't need to visit it recursively.

    निवड (e->kind) {
        स्थिती List_kind:
            VISIT_CONTAINER(e, List);
            परत रिक्त;
        स्थिती Tuple_kind:
            VISIT_CONTAINER(e, Tuple);
            परत रिक्त;
        स्थिती Starred_kind:
            जर (targets_type == DEL_TARGETS) {
                परत e;
            }
            परत _PyPegen_get_invalid_target(e->v.Starred.मूल्य, targets_type);
        स्थिती Compare_kind:
            // This is needed, because the `a in b` in `for a in b` gets parsed
            // as a comparison, and so we need to search the left side of the comparison
            // for invalid targets.
            जर (targets_type == FOR_TARGETS) {
                cmpop_ty cmpop = (cmpop_ty) asdl_seq_GET(e->v.Compare.ops, 0);
                जर (cmpop == In) {
                    परत _PyPegen_get_invalid_target(e->v.Compare.डावा, targets_type);
                }
                परत रिक्त;
            }
            परत e;
        स्थिती Name_kind:
        स्थिती Subscript_kind:
        स्थिती Attribute_kind:
            परत रिक्त;
        पूर्वनिर्धारित:
            परत e;
    }
}

शून्य *_PyPegen_arguments_parsing_error(Parser *p, अभिव्यक्ती_ty e) {
    पूर्णांक kwarg_unpacking = 0;
    साठी (Py_ssize_t i = 0, l = asdl_seq_LEN(e->v.Call.keywords); i < l; i++) {
        keyword_ty keyword = asdl_seq_GET(e->v.Call.keywords, i);
        जर (!keyword->arg) {
            kwarg_unpacking = 1;
        }
    }

    स्थिरांक अक्षर *संदेश = रिक्त;
    जर (kwarg_unpacking) {
        संदेश = "positional argument follows keyword argument unpacking";
    } अन्यथा {
        संदेश = "positional argument follows keyword argument";
    }

    परत RAISE_SYNTAX_ERROR(संदेश);
}

शून्य *
_PyPegen_nonparen_genexp_in_call(Parser *p, अभिव्यक्ती_ty args, asdl_comprehension_seq *comprehensions)
{
    /* The rule that calls this function is 'args for_if_clauses'.
       For the input f(L, x for x in y), L and x are in args and
       the for is parsed as a for_if_clause. We have to check if
       len <= 1, so that input like dict((a, b) for a, b in x)
       gets successfully parsed and then we pass the last
       argument (x in the above example) as the location of the
       error */
    Py_ssize_t len = asdl_seq_LEN(args->v.Call.args);
    जर (len <= 1) {
        परत रिक्त;
    }

    comprehension_ty last_comprehension = PyPegen_last_item(comprehensions, comprehension_ty);

    परत RAISE_SYNTAX_ERROR_KNOWN_RANGE(
        (अभिव्यक्ती_ty) asdl_seq_GET(args->v.Call.args, len - 1),
        _PyPegen_get_last_comprehension_item(last_comprehension),
        "Generator expression must be parenthesized"
    );
}

// Fstring stuff

स्थिर अभिव्यक्ती_ty
_PyPegen_decode_fstring_part(Parser* p, पूर्णांक is_raw, अभिव्यक्ती_ty constant, Token* token) {
    प्रतिज्ञा(PyUnicode_CheckExact(constant->v.Constant.मूल्य));

    स्थिरांक अक्षर* bstr = PyUnicode_AsUTF8(constant->v.Constant.मूल्य);
    जर (bstr == रिक्त) {
        परत रिक्त;
    }

    माप_प्रकार len;
    जर (म_भेद(bstr, "{{") == 0 || म_भेद(bstr, "}}") == 0) {
        len = 1;
    } अन्यथा {
        len = म_माप(bstr);
    }

    is_raw = is_raw || म_अक्षर(bstr, '\\') == रिक्त;
    PyObject *str = _PyPegen_decode_string(p, is_raw, bstr, len, token);
    जर (str == रिक्त) {
        _Pypegen_raise_decode_error(p);
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        परत रिक्त;
    }
    परत _PyAST_Constant(str, रिक्त, constant->lineno, constant->col_offset,
                           constant->end_lineno, constant->end_col_offset,
                           p->arena);
}

स्थिर asdl_अभिव्यक्ती_seq *
_get_resized_exprs(Parser *p, Token *a, asdl_अभिव्यक्ती_seq *raw_expressions,
                   Token *b, पूर्णांक is_tstring)
{
    Py_ssize_t n_वस्तुसंच = asdl_seq_LEN(raw_expressions);
    Py_ssize_t total_वस्तुसंच = n_वस्तुसंच;
    साठी (Py_ssize_t i = 0; i < n_वस्तुसंच; i++) {
        अभिव्यक्ती_ty item = asdl_seq_GET(raw_expressions, i);
        जर (item->kind == JoinedStr_kind) {
            total_वस्तुसंच += asdl_seq_LEN(item->v.JoinedStr.मूल्ये) - 1;
        }
    }

    स्थिरांक अक्षर* quote_str = PyBytes_AsString(a->bytes);
    जर (quote_str == रिक्त) {
        परत रिक्त;
    }
    पूर्णांक is_raw = strpbrk(quote_str, "rR") != रिक्त;

    asdl_अभिव्यक्ती_seq *seq = Py_asdl_अभिव्यक्ती_seq_new(total_वस्तुसंच, p->arena);
    जर (seq == रिक्त) {
        परत रिक्त;
    }

    Py_ssize_t index = 0;
    साठी (Py_ssize_t i = 0; i < n_वस्तुसंच; i++) {
        अभिव्यक्ती_ty item = asdl_seq_GET(raw_expressions, i);

        /* Debug expressions arrive as JoinedStr(text, value); flatten them
           into the surrounding string. */
        जर (item->kind == JoinedStr_kind) {
            asdl_अभिव्यक्ती_seq *मूल्ये = item->v.JoinedStr.मूल्ये;
            जर (asdl_seq_LEN(मूल्ये) != 2) {
                PyErr_Format(PyExc_SystemError,
                             is_tstring
                             ? "unexpected TemplateStr node without debug data in t-string at line %d"
                             : "unexpected JoinedStr node without debug data in f-string at line %d",
                             item->lineno);
                परत रिक्त;
            }

            अभिव्यक्ती_ty first = asdl_seq_GET(मूल्ये, 0);
            प्रतिज्ञा(first->kind == Constant_kind);
            asdl_seq_SET(seq, index++, first);

            अभिव्यक्ती_ty second = asdl_seq_GET(मूल्ये, 1);
            प्रतिज्ञा((is_tstring &&
                    second->kind == Interpolation_kind) ||
                   second->kind == Formatted_मूल्य_kind);
            asdl_seq_SET(seq, index++, second);

            सुरूठेवा;
        }

        जर (item->kind == Constant_kind) {
            item = _PyPegen_decode_fstring_part(p, is_raw, item, b);
            जर (item == रिक्त) {
                परत रिक्त;
            }

            /* Tokenizer emits string parts even when the underlying string
            might become an empty value (e.g. FSTRING_MIDDLE with the value \\n)
            so we need to check for them and simplify it here. */
            जर (PyUnicode_CheckExact(item->v.Constant.मूल्य)
                && PyUnicode_GET_LENGTH(item->v.Constant.मूल्य) == 0) {
                सुरूठेवा;
            }
        }
        asdl_seq_SET(seq, index++, item);
    }

    asdl_अभिव्यक्ती_seq *resized_exprs;
    जर (index != total_वस्तुसंच) {
        resized_exprs = Py_asdl_अभिव्यक्ती_seq_new(index, p->arena);
        जर (resized_exprs == रिक्त) {
            परत रिक्त;
        }
        साठी (Py_ssize_t i = 0; i < index; i++) {
            asdl_seq_SET(resized_exprs, i, asdl_seq_GET(seq, i));
        }
    }
    अन्यथा {
        resized_exprs = seq;
    }
    परत resized_exprs;
}

अभिव्यक्ती_ty
_PyPegen_template_str(Parser *p, Token *a, asdl_अभिव्यक्ती_seq *raw_expressions, Token *b) {

    asdl_अभिव्यक्ती_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 1);
    जर (resized_exprs == रिक्त) {
        परत रिक्त;
    }
    परत _PyAST_TemplateStr(resized_exprs, a->lineno, a->col_offset,
                              b->end_lineno, b->end_col_offset,
                              p->arena);
}

अभिव्यक्ती_ty
_PyPegen_joined_str(Parser *p, Token* a, asdl_अभिव्यक्ती_seq* raw_expressions, Token*b) {

    asdl_अभिव्यक्ती_seq *resized_exprs = _get_resized_exprs(p, a, raw_expressions, b, 0);
    जर (resized_exprs == रिक्त) {
        परत रिक्त;
    }
    परत _PyAST_JoinedStr(resized_exprs, a->lineno, a->col_offset,
                            b->end_lineno, b->end_col_offset,
                            p->arena);
}

अभिव्यक्ती_ty _PyPegen_decoded_constant_from_token(Parser* p, Token* tok) {
    जर (tok->bytes == रिक्त) {
        परत Py_Pegen_raise_incomplete_आदान(p);
    }
    Py_ssize_t bsize = PyBytes_GET_SIZE(tok->bytes);
    स्थिरांक अक्षर *bstr = PyBytes_AS_STRING(tok->bytes);

    पूर्णांक is_raw = tok->is_raw;

    PyObject* str = _PyPegen_decode_string(p, is_raw, bstr, bsize, tok);
    जर (str == रिक्त) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        परत रिक्त;
    }
    परत _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

अभिव्यक्ती_ty _PyPegen_constant_from_token(Parser* p, Token* tok) {
    जर (tok->bytes == रिक्त) {
        परत Py_Pegen_raise_incomplete_आदान(p);
    }
    स्थिरांक अक्षर *bstr = PyBytes_AS_STRING(tok->bytes);
    PyObject* str = PyUnicode_FromString(bstr);
    जर (str == रिक्त) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, str) < 0) {
        Py_DECREF(str);
        परत रिक्त;
    }
    परत _PyAST_Constant(str, रिक्त, tok->lineno, tok->col_offset,
                           tok->end_lineno, tok->end_col_offset,
                           p->arena);
}

अभिव्यक्ती_ty _PyPegen_constant_from_string(Parser* p, Token* tok) {
    जर (tok->bytes == रिक्त) {
        परत Py_Pegen_raise_incomplete_आदान(p);
    }
    स्थिरांक अक्षर *the_str = PyBytes_AS_STRING(tok->bytes);
    PyObject *s = _PyPegen_parse_string(p, tok);
    जर (s == रिक्त) {
        _Pypegen_raise_decode_error(p);
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, s) < 0) {
        Py_DECREF(s);
        परत रिक्त;
    }
    PyObject *kind = रिक्त;
    जर (the_str && the_str[0] == 'u') {
        kind = _PyPegen_new_identifier(p, "u");
        जर (kind == रिक्त) {
            परत रिक्त;
        }
    }
    परत _PyAST_Constant(s, kind, tok->lineno, tok->col_offset, tok->end_lineno, tok->end_col_offset, p->arena);
}

स्थिर पूर्णांक
_get_interpolation_conversion(Parser *p, Token *debug, ResultTokenWithMetadata *conversion,
                              ResultTokenWithMetadata *format)
{
    जर (conversion != रिक्त) {
        अभिव्यक्ती_ty conversion_अभिव्यक्ती = (अभिव्यक्ती_ty) conversion->result;
        प्रतिज्ञा(conversion_अभिव्यक्ती->kind == Name_kind);
        Py_UCS4 first = PyUnicode_READ_CHAR(conversion_अभिव्यक्ती->v.Name.id, 0);
        परत Py_SAFE_DOWNCAST(first, Py_UCS4, पूर्णांक);
    }
    अन्यथा जर (debug && !format) {
        /* If no conversion is specified, use !r for debug expressions */
        परत (पूर्णांक)'r';
    }
    परत -1;
}

स्थिर PyObject *
strip_interpolation_debug_अभिव्यक्ती(PyObject *exprstr)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(exprstr);

    /* Discard whitespace and explicit line continuations after the debug "="
       but preserve whitespace before it. */
    पर्यंत (len > 0) {
        पूर्णांक has_newline = 0;
        पर्यंत (len > 0) {
            Py_UCS4 c = PyUnicode_READ_CHAR(exprstr, len - 1);
            जर (!_PyUnicode_IsWhitespace(c)) {
                थांबा;
            }
            जर (c == '\r' || c == '\n') {
                has_newline = 1;
            }
            len--;
        }
        जर (!has_newline || len == 0 ||
            PyUnicode_READ_CHAR(exprstr, len - 1) != '\\')
        {
            थांबा;
        }
        len--;
    }

    /* Preserve unexpected metadata instead of dropping source text. */
    जर (len == 0 || PyUnicode_READ_CHAR(exprstr, len - 1) != '=') {
        परत Py_NewRef(exprstr);
    }

    परत PyUnicode_Substring(exprstr, 0, len - 1);
}

अभिव्यक्ती_ty _PyPegen_interpolation(Parser *p, अभिव्यक्ती_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णांक lineno, पूर्णांक col_offset,
                                 पूर्णांक end_lineno, पूर्णांक end_col_offset, PyArena *arena) {

    पूर्णांक conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    /* Find the non whitespace token after the "=" */
    पूर्णांक debug_end_ओळ, debug_end_offset;
    PyObject *debug_metadata;
    constant exprstr;

    जर (conversion) {
        debug_end_ओळ = ((अभिव्यक्ती_ty) conversion->result)->lineno;
        debug_end_offset = ((अभिव्यक्ती_ty) conversion->result)->col_offset;
        debug_metadata = exprstr = conversion->metadata;
    }
    अन्यथा जर (format) {
        debug_end_ओळ = ((अभिव्यक्ती_ty) format->result)->lineno;
        debug_end_offset = ((अभिव्यक्ती_ty) format->result)->col_offset + 1;
        debug_metadata = exprstr = format->metadata;
    }
    अन्यथा {
        debug_end_ओळ = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = exprstr = closing_brace->metadata;
    }

    प्रतिज्ञा(exprstr != रिक्त);
    PyObject *final_exprstr = debug
        ? strip_interpolation_debug_अभिव्यक्ती(exprstr)
        : Py_NewRef(exprstr);
    जर (!final_exprstr || _PyArena_AddPyObject(arena, final_exprstr) < 0) {
        Py_XDECREF(final_exprstr);
        परत रिक्त;
    }

    अभिव्यक्ती_ty interpolation = _PyAST_Interpolation(
        expression, final_exprstr, conversion_val, format ? (अभिव्यक्ती_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    जर (!interpolation || !debug) {
        परत interpolation;
    }

    अभिव्यक्ती_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_ओळ,
                                            debug_end_offset - 1, p->arena);
    जर (!debug_text) {
        परत रिक्त;
    }

    asdl_अभिव्यक्ती_seq *मूल्ये = Py_asdl_अभिव्यक्ती_seq_new(2, arena);
    जर (!मूल्ये) {
        परत रिक्त;
    }
    asdl_seq_SET(मूल्ये, 0, debug_text);
    asdl_seq_SET(मूल्ये, 1, interpolation);
    परत _PyAST_JoinedStr(मूल्ये, lineno, col_offset, debug_end_ओळ, debug_end_offset, p->arena);
}

अभिव्यक्ती_ty Py_Pegen_formatted_मूल्य(Parser *p, अभिव्यक्ती_ty expression, Token *debug, ResultTokenWithMetadata *conversion,
                                 ResultTokenWithMetadata *format, Token *closing_brace, पूर्णांक lineno, पूर्णांक col_offset,
                                 पूर्णांक end_lineno, पूर्णांक end_col_offset, PyArena *arena) {
    पूर्णांक conversion_val = _get_interpolation_conversion(p, debug, conversion, format);

    अभिव्यक्ती_ty formatted_मूल्य = Py_AST_Formatted_मूल्य(
        expression, conversion_val, format ? (अभिव्यक्ती_ty) format->result : रिक्त,
        lineno, col_offset, end_lineno,
        end_col_offset, arena
    );

    जर (!formatted_मूल्य || !debug) {
        परत formatted_मूल्य;
    }

    /* Find the non whitespace token after the "=" */
    पूर्णांक debug_end_ओळ, debug_end_offset;
    PyObject *debug_metadata;

    जर (conversion) {
        debug_end_ओळ = ((अभिव्यक्ती_ty) conversion->result)->lineno;
        debug_end_offset = ((अभिव्यक्ती_ty) conversion->result)->col_offset;
        debug_metadata = conversion->metadata;
    }
    अन्यथा जर (format) {
        debug_end_ओळ = ((अभिव्यक्ती_ty) format->result)->lineno;
        debug_end_offset = ((अभिव्यक्ती_ty) format->result)->col_offset + 1;
        debug_metadata = format->metadata;
    }
    अन्यथा {
        debug_end_ओळ = end_lineno;
        debug_end_offset = end_col_offset;
        debug_metadata = closing_brace->metadata;
    }
    अभिव्यक्ती_ty debug_text = _PyAST_Constant(debug_metadata, रिक्त, lineno, col_offset + 1, debug_end_ओळ,
                                            debug_end_offset - 1, p->arena);
    जर (!debug_text) {
        परत रिक्त;
    }

    asdl_अभिव्यक्ती_seq *मूल्ये = Py_asdl_अभिव्यक्ती_seq_new(2, arena);
    जर (!मूल्ये) {
        परत रिक्त;
    }
    asdl_seq_SET(मूल्ये, 0, debug_text);
    asdl_seq_SET(मूल्ये, 1, formatted_मूल्य);
    परत _PyAST_JoinedStr(मूल्ये, lineno, col_offset, debug_end_ओळ, debug_end_offset, p->arena);
}

स्थिर अभिव्यक्ती_ty
_build_concatenated_bytes(Parser *p, asdl_अभिव्यक्ती_seq *strings, पूर्णांक lineno,
                        पूर्णांक col_offset, पूर्णांक end_lineno, पूर्णांक end_col_offset,
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
    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मूल्य;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        जर (part > PY_SSIZE_T_MAX - total) {
            PyErr_NoMemory();
            परत रिक्त;
        }
        total += part;
    }

    PyBytesWriter *writer = PyBytesWriter_Create(total);
    जर (writer == रिक्त) {
        परत रिक्त;
    }
    अक्षर *out = PyBytesWriter_GetData(writer);

    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(strings, i);
        PyObject *bytes = elem->v.Constant.मूल्य;
        Py_ssize_t part = PyBytes_GET_SIZE(bytes);
        जर (part > 0) {
            स_नकल(out, PyBytes_AS_STRING(bytes), part);
            out += part;
        }
    }

    PyObject *res = PyBytesWriter_Finish(writer);
    जर (res == रिक्त) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(arena, res) < 0) {
        Py_DECREF(res);
        परत रिक्त;
    }
    परत _PyAST_Constant(res, kind, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

स्थिर अभिव्यक्ती_ty
_build_concatenated_unicode(Parser *p, asdl_अभिव्यक्ती_seq *strings, पूर्णांक lineno,
                        पूर्णांक col_offset, पूर्णांक end_lineno, पूर्णांक end_col_offset,
                        PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 1);

    अभिव्यक्ती_ty first = asdl_seq_GET(strings, 0);

    /* When a string is getting concatenated, the kind of the string
        is determined by the first string in the concatenation
        sequence.

        u"abc" "def" -> u"abcdef"
        "abc" u"abc" ->  "abcabc" */
    PyObject *kind = first->v.Constant.kind;

    PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
    जर (writer == रिक्त) {
        परत रिक्त;
    }

    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty current_elem = asdl_seq_GET(strings, i);
        प्रतिज्ञा(current_elem->kind == Constant_kind);

        जर (PyUnicodeWriter_WriteStr(writer,
                                     current_elem->v.Constant.मूल्य)) {
            PyUnicodeWriter_Discard(writer);
            परत रिक्त;
        }
    }

    PyObject *final = PyUnicodeWriter_Finish(writer);
    जर (final == रिक्त) {
        परत रिक्त;
    }
    जर (_PyArena_AddPyObject(p->arena, final) < 0) {
        Py_DECREF(final);
        परत रिक्त;
    }
    परत _PyAST_Constant(final, kind, lineno, col_offset,
                           end_lineno, end_col_offset, arena);
}

स्थिर asdl_अभिव्यक्ती_seq *
_build_concatenated_str(Parser *p, asdl_अभिव्यक्ती_seq *strings,
                               पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                               पूर्णांक end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    Py_ssize_t n_flattened_elements = 0;
    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(strings, i);
        निवड(elem->kind) {
            स्थिती JoinedStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.JoinedStr.मूल्ये);
                थांबा;
            स्थिती TemplateStr_kind:
                n_flattened_elements += asdl_seq_LEN(elem->v.TemplateStr.मूल्ये);
                थांबा;
            पूर्वनिर्धारित:
                n_flattened_elements++;
                थांबा;
        }
    }


    asdl_अभिव्यक्ती_seq* flattened = Py_asdl_अभिव्यक्ती_seq_new(n_flattened_elements, p->arena);
    जर (flattened == रिक्त) {
        परत रिक्त;
    }

    /* build flattened list */
    Py_ssize_t current_pos = 0;
    साठी (Py_ssize_t i = 0; i < len; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(strings, i);
        निवड(elem->kind) {
            स्थिती JoinedStr_kind:
                साठी (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.JoinedStr.मूल्ये); j++) {
                    अभिव्यक्ती_ty subvalue = asdl_seq_GET(elem->v.JoinedStr.मूल्ये, j);
                    जर (subvalue == रिक्त) {
                        परत रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                थांबा;
            स्थिती TemplateStr_kind:
                साठी (Py_ssize_t j = 0; j < asdl_seq_LEN(elem->v.TemplateStr.मूल्ये); j++) {
                    अभिव्यक्ती_ty subvalue = asdl_seq_GET(elem->v.TemplateStr.मूल्ये, j);
                    जर (subvalue == रिक्त) {
                        परत रिक्त;
                    }
                    asdl_seq_SET(flattened, current_pos++, subvalue);
                }
                थांबा;
            पूर्वनिर्धारित:
                asdl_seq_SET(flattened, current_pos++, elem);
                थांबा;
        }
    }

    /* calculate folded element count */
    Py_ssize_t n_elements = 0;
    पूर्णांक prev_is_constant = 0;
    साठी (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(flattened, i);

        /* The concatenation of a FormattedValue and an empty Constant should
           lead to the FormattedValue itself. Thus, we will not take any empty
           constants into account, just as in `_PyPegen_joined_str` */
        जर (elem->kind == Constant_kind &&
            PyUnicode_CheckExact(elem->v.Constant.मूल्य) &&
            PyUnicode_GET_LENGTH(elem->v.Constant.मूल्य) == 0)
            सुरूठेवा;

        जर (!prev_is_constant || elem->kind != Constant_kind) {
            n_elements++;
        }
        prev_is_constant = elem->kind == Constant_kind;
    }

    asdl_अभिव्यक्ती_seq* मूल्ये = Py_asdl_अभिव्यक्ती_seq_new(n_elements, p->arena);
    जर (मूल्ये == रिक्त) {
        परत रिक्त;
    }

    /* build folded list */
    current_pos = 0;
    साठी (Py_ssize_t i = 0; i < n_flattened_elements; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(flattened, i);

        /* if the current elem and the following are constants,
           fold them and all consequent constants */
        जर (elem->kind == Constant_kind) {
            जर (i + 1 < n_flattened_elements &&
                asdl_seq_GET(flattened, i + 1)->kind == Constant_kind) {
                अभिव्यक्ती_ty first_elem = elem;

                /* When a string is getting concatenated, the kind of the string
                   is determined by the first string in the concatenation
                   sequence.

                   u"abc" "def" -> u"abcdef"
                   "abc" u"abc" ->  "abcabc" */
                PyObject *kind = elem->v.Constant.kind;

                PyUnicodeWriter *writer = PyUnicodeWriter_Create(0);
                जर (writer == रिक्त) {
                    परत रिक्त;
                }
                अभिव्यक्ती_ty last_elem = elem;
                Py_ssize_t j;
                साठी (j = i; j < n_flattened_elements; j++) {
                    अभिव्यक्ती_ty current_elem = asdl_seq_GET(flattened, j);
                    जर (current_elem->kind == Constant_kind) {
                        जर (PyUnicodeWriter_WriteStr(writer,
                                                     current_elem->v.Constant.मूल्य)) {
                            PyUnicodeWriter_Discard(writer);
                            परत रिक्त;
                        }
                        last_elem = current_elem;
                    } अन्यथा {
                        थांबा;
                    }
                }
                i = j - 1;

                PyObject *concat_str = PyUnicodeWriter_Finish(writer);
                जर (concat_str == रिक्त) {
                    परत रिक्त;
                }
                जर (_PyArena_AddPyObject(p->arena, concat_str) < 0) {
                    Py_DECREF(concat_str);
                    परत रिक्त;
                }
                elem = _PyAST_Constant(concat_str, kind, first_elem->lineno,
                                       first_elem->col_offset,
                                       last_elem->end_lineno,
                                       last_elem->end_col_offset, p->arena);
                जर (elem == रिक्त) {
                    परत रिक्त;
                }
            }

            /* Drop all empty contanst strings */
            जर (PyUnicode_CheckExact(elem->v.Constant.मूल्य) &&
                PyUnicode_GET_LENGTH(elem->v.Constant.मूल्य) == 0) {
                सुरूठेवा;
            }
        }

        asdl_seq_SET(मूल्ये, current_pos++, elem);
    }

    प्रतिज्ञा(current_pos == n_elements);
    परत मूल्ये;
}

स्थिर अभिव्यक्ती_ty
_build_concatenated_joined_str(Parser *p, asdl_अभिव्यक्ती_seq *strings,
                               पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                               पूर्णांक end_col_offset, PyArena *arena)
{
    asdl_अभिव्यक्ती_seq *मूल्ये = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    जर (!मूल्ये) {
        परत रिक्त;
    }
    परत _PyAST_JoinedStr(मूल्ये, lineno, col_offset, end_lineno, end_col_offset, p->arena);
}

अभिव्यक्ती_ty
_PyPegen_concatenate_tstrings(Parser *p, asdl_अभिव्यक्ती_seq *strings,
                               पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                               पूर्णांक end_col_offset, PyArena *arena)
{
    asdl_अभिव्यक्ती_seq *मूल्ये = _build_concatenated_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
    जर (!मूल्ये) {
        परत रिक्त;
    }
    परत _PyAST_TemplateStr(मूल्ये, lineno, col_offset, end_lineno,
        end_col_offset, arena);
}

अभिव्यक्ती_ty
_PyPegen_concatenate_strings(Parser *p, asdl_अभिव्यक्ती_seq *strings,
                             पूर्णांक lineno, पूर्णांक col_offset, पूर्णांक end_lineno,
                             पूर्णांक end_col_offset, PyArena *arena)
{
    Py_ssize_t len = asdl_seq_LEN(strings);
    प्रतिज्ञा(len > 0);

    पूर्णांक f_string_found = 0;
    पूर्णांक unicode_string_found = 0;
    पूर्णांक bytes_found = 0;

    Py_ssize_t i = 0;
    साठी (i = 0; i < len; i++) {
        अभिव्यक्ती_ty elem = asdl_seq_GET(strings, i);
        निवड(elem->kind) {
            स्थिती Constant_kind:
                जर (PyBytes_CheckExact(elem->v.Constant.मूल्य)) {
                    bytes_found = 1;
                } अन्यथा {
                    unicode_string_found = 1;
                }
                थांबा;
            स्थिती JoinedStr_kind:
                f_string_found = 1;
                थांबा;
            स्थिती TemplateStr_kind:
                // python.gram handles this; we should never get here
                प्रतिज्ञा(0);
                थांबा;
            पूर्वनिर्धारित:
                f_string_found = 1;
                थांबा;
        }
    }

    // Cannot mix unicode and bytes
    जर ((unicode_string_found || f_string_found) && bytes_found) {
        RAISE_SYNTAX_ERROR("cannot mix bytes and nonbytes literals");
        परत रिक्त;
    }

    // If it's only bytes or only unicode string, do a simple concat
    जर (!f_string_found) {
        जर (len == 1) {
            परत asdl_seq_GET(strings, 0);
        }
        अन्यथा जर (bytes_found) {
            परत _build_concatenated_bytes(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
        अन्यथा {
            परत _build_concatenated_unicode(p, strings, lineno, col_offset,
                end_lineno, end_col_offset, arena);
        }
    }

    परत _build_concatenated_joined_str(p, strings, lineno,
        col_offset, end_lineno, end_col_offset, arena);
}

स्थिर पूर्णांक
_warn_relative_import_of_lazy(Parser *p, asdl_seq *dots, अभिव्यक्ती_ty module)
{
    // Warn about `from . lazy import x`: the whitespace between the dots and
    // the module name is insignificant, so this is parsed exactly like
    // `from .lazy import x` (an import of the relative module "lazy"), but it
    // is most likely a transposition of `lazy from . import x` (PEP 810).
    जर (p->call_invalid_rules) {
        परत 0;
    }

    // Only fire if there is whitespace between the last dot and the name,
    // i.e. not for the common `from .lazy import x` spelling.
    Token *last_dot = asdl_seq_GET_UNTYPED(dots, asdl_seq_LEN(dots) - 1);
    जर (
        last_dot->end_lineno == module->lineno
        && last_dot->end_col_offset == module->col_offset
    ) {
        परत 0;
    }

    पूर्णांक मोजणी = Py_Pegen_seq_मोजणी_dots(dots);
    अक्षर *buf = PyMem_RawMalloc(मोजणी + 1);
    जर (buf == रिक्त) {
        PyErr_NoMemory();
        परत -1;
    }
    स_रखो(buf, '.', मोजणी);
    buf[मोजणी] = '\0';

    PyObject *संदेश = PyUnicode_FromFormat(
        "'from %s lazy import' is the same as 'from %slazy import'; "
        "did you mean 'lazy from %s import'?",
        buf, buf, buf);
    PyMem_RawFree(buf);
    जर (संदेश == रिक्त) {
        परत -1;
    }

    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    पूर्णांक res = _PyErr_EmitSyntaxWarning(संदेश,
                                       info.filename,
                                       module->lineno,
                                       module->col_offset + 1,
                                       module->end_lineno,
                                       module->end_col_offset + 1,
                                       info.module);
    Py_DECREF(संदेश);
    परत res;
}

stmt_ty
_PyPegen_checked_from_import(Parser *p, asdl_seq *dots, अभिव्यक्ती_ty module_name,
                             asdl_alias_seq *names, अभिव्यक्ती_ty lazy_token, पूर्णांक lineno,
                             पूर्णांक col_offset, पूर्णांक end_lineno, पूर्णांक end_col_offset,
                             PyArena *arena)
{
    identifier module = module_name->v.Name.id;
    पूर्णांक level = Py_Pegen_seq_मोजणी_dots(dots);
    जर (level == 0 && PyUnicode_CompareWithASCIIString(module, "__future__") == 0) {
        जर (lazy_token) {
            RAISE_SYNTAX_ERROR_KNOWN_LOCATION(lazy_token,
                "lazy from __future__ import is not allowed");
            परत रिक्त;
        }
        साठी (Py_ssize_t i = 0; i < asdl_seq_LEN(names); i++) {
            alias_ty alias = asdl_seq_GET(names, i);
            जर (PyUnicode_CompareWithASCIIString(alias->name, "barry_as_FLUFL") == 0) {
                p->flags |= PyPARSE_BARRY_AS_BDFL;
                p->tok->barry_as_bdfl = 1;
            }
        }
    }
    अन्यथा जर (
        level > 0
        && lazy_token == रिक्त
        && PyUnicode_CompareWithASCIIString(module, "lazy") == 0
    ) {
        जर (_warn_relative_import_of_lazy(p, dots, module_name) < 0) {
            परत रिक्त;
        }
    }
    परत _PyAST_ImportFrom(module, names, level, lazy_token ? 1 : 0, lineno,
                             col_offset, end_lineno, end_col_offset, arena);
}

asdl_stmt_seq*
_PyPegen_register_stmts(Parser *p, asdl_stmt_seq* stmts) {
    जर (!p->call_invalid_rules) {
        परत stmts;
    }
    Py_ssize_t len = asdl_seq_LEN(stmts);
    जर (len == 0) {
        परत stmts;
    }
    stmt_ty last_stmt = asdl_seq_GET(stmts, len - 1);
    जर (p->last_stmt_location.lineno > last_stmt->lineno) {
        परत stmts;
    }
    p->last_stmt_location.lineno = last_stmt->lineno;
    p->last_stmt_location.col_offset = last_stmt->col_offset;
    p->last_stmt_location.end_lineno = last_stmt->end_lineno;
    p->last_stmt_location.end_col_offset = last_stmt->end_col_offset;
    परत stmts;
}
