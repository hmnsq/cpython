#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const char *str, PyObject* filename, цео mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ако (PySys_Audit("compile", "yO", str, filename) < 0) {
        врати NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    врати result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                      цео mode, const char *ps1, const char* ps2,
                      PyCompilerFlags *flags, цео *errcode, PyArena *arena)
{
    ако (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        врати NULL;
    }
    врати _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const char *enc,
                                 цео mode, const char *ps1, const char* ps2,
                                 PyCompilerFlags *flags, цео *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ако (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        врати NULL;
    }
    врати _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
