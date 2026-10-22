#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, ціле mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    якщо (PySys_Audit("compile", "yO", str, filename) < 0) {
        повернути NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    повернути result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      ціле mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, ціле *errcode, PyArena *arena)
{
    якщо (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        повернути NULL;
    }
    повернути _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 ціле mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, ціле *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    якщо (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        повернути NULL;
    }
    повернути _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
