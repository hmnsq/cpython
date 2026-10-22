#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, ကိန်းပြည့် mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    အကယ်၍ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ပြန်ပေး NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ပြန်ပေး result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      ကိန်းပြည့် mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, ကိန်းပြည့် *errcode, PyArena *arena)
{
    အကယ်၍ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 ကိန်းပြည့် mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, ကိန်းပြည့် *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    အကယ်၍ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ပြန်ပေး NULL;
    }
    ပြန်ပေး _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
