#અન્તર્ભાવ "Python.h"

#અન્તર્ભાવ "pegen.h"

mod_ty
_PyParser_ASTFromString(અચલ અક્ષર *str, PyObject* filename, પૂર્ણાંક mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    જો (PySys_Audit("compile", "yO", str, filename) < 0) {
        પાછા ખાલી;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    પાછા result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, અચલ અક્ષર *enc,
                      પૂર્ણાંક mode, અચલ અક્ષર *ps1, અચલ અક્ષર* ps2,
                      PyCompilerFlags *flags, પૂર્ણાંક *errcode, PyArena *arena)
{
    જો (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        પાછા ખાલી;
    }
    પાછા _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ખાલી, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, અચલ અક્ષર *enc,
                                 પૂર્ણાંક mode, અચલ અક્ષર *ps1, અચલ અક્ષર* ps2,
                                 PyCompilerFlags *flags, પૂર્ણાંક *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    જો (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        પાછા ખાલી;
    }
    પાછા _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
