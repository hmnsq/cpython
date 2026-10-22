#include "Python.h"

#include "pegen.h"

mod_ty
_PyParser_ASTFromString(const var_na *str, PyObject* filename, ຈຳນວນເຕັມ mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ຖ້າ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ສົ່ງຄືນ NULL;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ສົ່ງຄືນ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                      ຈຳນວນເຕັມ mode, const var_na *ps1, const var_na* ps2,
                      PyCompilerFlags *flags, ຈຳນວນເຕັມ *errcode, PyArena *arena)
{
    ຖ້າ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, NULL, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, const var_na *enc,
                                 ຈຳນວນເຕັມ mode, const var_na *ps1, const var_na* ps2,
                                 PyCompilerFlags *flags, ຈຳນວນເຕັມ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ຖ້າ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ສົ່ງຄືນ NULL;
    }
    ສົ່ງຄືນ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
