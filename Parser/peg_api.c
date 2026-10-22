#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, целое mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    если (PySys_Audit("compile", "yO", str, filename) < 0) {
        вернуть NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    вернуть result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      целое mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, целое *errcode, PyArena *arena)
{
    если (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        вернуть NULL;
    }
    вернуть _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 целое mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, целое *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    если (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        вернуть NULL;
    }
    вернуть _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
