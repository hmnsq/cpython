#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, puur_naa_nka mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ပေႃး (PySys_Audit("compile", "yO", str, filename) < 0) {
        ပွၵ်ႈ NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ပွၵ်ႈ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      puur_naa_nka mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, puur_naa_nka *errcode, PyArena *arena)
{
    ပေႃး (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ပွၵ်ႈ NULL;
    }
    ပွၵ်ႈ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 puur_naa_nka mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, puur_naa_nka *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ပေႃး (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ပွၵ်ႈ NULL;
    }
    ပွၵ်ႈ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
