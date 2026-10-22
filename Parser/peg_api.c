#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, 整数 mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    如果 (PySys_Audit("compile", "yO", str, filename) < 0) {
        返回 NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    返回 result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      整数 mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, 整数 *errcode, PyArena *arena)
{
    如果 (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        返回 NULL;
    }
    返回 _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 整数 mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, 整数 *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    如果 (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        返回 NULL;
    }
    返回 _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
