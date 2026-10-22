#অন্তর্ভাব "Python.h"

#অন্তর্ভাব "pegen.h"

mod_ty
_PyParser_ASTFromString(ধ্রুব বর্ণ *str, PyObject* filename, পূর্ণাঙ্ক mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    যদি (PySys_Audit("compile", "yO", str, filename) < 0) {
        প্রত্যাগম রিক্ত;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    প্রত্যাগম result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুব বর্ণ *enc,
                      পূর্ণাঙ্ক mode, ধ্রুব বর্ণ *ps1, ধ্রুব বর্ণ* ps2,
                      PyCompilerFlags *flags, পূর্ণাঙ্ক *errcode, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, রিক্ত, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুব বর্ণ *enc,
                                 পূর্ণাঙ্ক mode, ধ্রুব বর্ণ *ps1, ধ্রুব বর্ণ* ps2,
                                 PyCompilerFlags *flags, পূর্ণাঙ্ক *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
