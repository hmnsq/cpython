#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, цяло mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ако (PySys_Audit("compile", "yO", str, filename) < 0) {
        върни NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    върни result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      цяло mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, цяло *errcode, PyArena *arena)
{
    ако (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        върни NULL;
    }
    върни _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 цяло mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, цяло *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ако (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        върни NULL;
    }
    върни _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
