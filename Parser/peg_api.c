#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, số_nguyên mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    nếu (PySys_Audit("compile", "yO", str, filename) < 0) {
        trả_về NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    trả_về result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      số_nguyên mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, số_nguyên *errcode, PyArena *arena)
{
    nếu (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        trả_về NULL;
    }
    trả_về _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 số_nguyên mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, số_nguyên *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    nếu (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        trả_về NULL;
    }
    trả_về _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
