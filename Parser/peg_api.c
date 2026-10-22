#ضمن "Python.h"

#ضمن "pegen.h"

mod_ty
_PyParser_ASTFromString(ثابت حرف *str, PyObject* filename, صحيح نما,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    إذا (PySys_Audit("compile", "yO", str, filename) < 0) {
        أرجع عدم;
    }

    mod_ty نتیجه = Py_Pegen_پیاپی_parser_from_string(str, نما, filename, flags,
                                                    arena, module, incomplete);
    أرجع نتیجه;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                      صحيح نما, ثابت حرف *ps1, ثابت حرف* ps2,
                      PyCompilerFlags *flags, صحيح *errcode, PyArena *arena)
{
    إذا (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        أرجع عدم;
    }
    أرجع Py_Pegen_پیاپی_parser_from_file_اشاره_گر(fp, نما, filename_ob, enc, ps1, ps2,
                                        flags, errcode, عدم, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                                 صحيح نما, ثابت حرف *ps1, ثابت حرف* ps2,
                                 PyCompilerFlags *flags, صحيح *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    إذا (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        أرجع عدم;
    }
    أرجع Py_Pegen_پیاپی_parser_from_file_اشاره_گر(fp, نما, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
