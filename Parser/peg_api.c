#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, පූර්ණ_සංඛ්‍යා mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    නම් (PySys_Audit("compile", "yO", str, filename) < 0) {
        ආපසු NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ආපසු result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      පූර්ණ_සංඛ්‍යා mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, පූර්ණ_සංඛ්‍යා *errcode, PyArena *arena)
{
    නම් (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ආපසු NULL;
    }
    ආපසු _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 පූර්ණ_සංඛ්‍යා mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, පූර්ණ_සංඛ්‍යා *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    නම් (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ආපසු NULL;
    }
    ආපසු _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
