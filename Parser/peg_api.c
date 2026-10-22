#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, ཧྲིལ mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    གལ་སྲིད (PySys_Audit("compile", "yO", str, filename) < 0) {
        ལོག NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ལོག result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      ཧྲིལ mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, ཧྲིལ *errcode, PyArena *arena)
{
    གལ་སྲིད (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ལོག NULL;
    }
    ལོག _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 ཧྲིལ mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, ཧྲིལ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    གལ་སྲིད (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ལོག NULL;
    }
    ལོག _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
