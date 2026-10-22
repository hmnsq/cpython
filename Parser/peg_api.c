#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, จำนวนเต็ม mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ถ้า (PySys_Audit("compile", "yO", str, filename) < 0) {
        คืน NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    คืน result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      จำนวนเต็ม mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, จำนวนเต็ม *errcode, PyArena *arena)
{
    ถ้า (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        คืน NULL;
    }
    คืน _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 จำนวนเต็ม mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, จำนวนเต็ม *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ถ้า (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        คืน NULL;
    }
    คืน _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
