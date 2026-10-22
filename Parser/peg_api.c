#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, 정수 mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    만약 (PySys_Audit("compile", "yO", str, filename) < 0) {
        반환 NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    반환 result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      정수 mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, 정수 *errcode, PyArena *arena)
{
    만약 (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        반환 NULL;
    }
    반환 _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 정수 mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, 정수 *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    만약 (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        반환 NULL;
    }
    반환 _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
