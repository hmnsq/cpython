#অন্তর্ভাৱ "Python.h"

#অন্তর্ভাৱ "pegen.h"

mod_ty
_PyParser_ASTFromString(ধ্রুৱ ৱর্ণ *str, PyObject* filename, পূর্ণাঙ্ক mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    যদি (PySys_Audit("compile", "yO", str, filename) < 0) {
        ঘূৰাই রিক্ত;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ঘূৰাই result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুৱ ৱর্ণ *enc,
                      পূর্ণাঙ্ক mode, ধ্রুৱ ৱর্ণ *ps1, ধ্রুৱ ৱর্ণ* ps2,
                      PyCompilerFlags *flags, পূর্ণাঙ্ক *errcode, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, রিক্ত, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুৱ ৱর্ণ *enc,
                                 পূর্ণাঙ্ক mode, ধ্রুৱ ৱর্ণ *ps1, ধ্রুৱ ৱর্ণ* ps2,
                                 PyCompilerFlags *flags, পূর্ণাঙ্ক *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ঘূৰাই রিক্ত;
    }
    ঘূৰাই _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
