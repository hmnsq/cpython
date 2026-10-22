#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, ចំនួនគត់ mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ប្រសិនបើ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ត្រឡប់ NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ត្រឡប់ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      ចំនួនគត់ mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, ចំនួនគត់ *errcode, PyArena *arena)
{
    ប្រសិនបើ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 ចំនួនគត់ mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, ចំនួនគត់ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ប្រសិនបើ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ត្រឡប់ NULL;
    }
    ត្រឡប់ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
