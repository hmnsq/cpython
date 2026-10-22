#অন্তর্ভাব "Python.h"

#অন্তর্ভাব "pegen.h"

mod_ty
_PyParser_ASTFromString(ধ্রুবক অক্ষর *str, PyObject* filename, পূর্ণসংখ্যা mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    যদি (PySys_Audit("compile", "yO", str, filename) < 0) {
        ফেরত ফাঁকা;
    }

    mod_ty ফলাফল = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ফেরত ফলাফল;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুবক অক্ষর *enc,
                      পূর্ণসংখ্যা mode, ধ্রুবক অক্ষর *ps1, ধ্রুবক অক্ষর* ps2,
                      PyCompilerFlags *flags, পূর্ণসংখ্যা *errcode, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ফাঁকা, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ধ্রুবক অক্ষর *enc,
                                 পূর্ণসংখ্যা mode, ধ্রুবক অক্ষর *ps1, ধ্রুবক অক্ষর* ps2,
                                 PyCompilerFlags *flags, পূর্ণসংখ্যা *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    যদি (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ফেরত ফাঁকা;
    }
    ফেরত _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
