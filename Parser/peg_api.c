#شامل "Python.h"

#شامل "pegen.h"

mod_ty
_PyParser_ASTFromString(ثابت حرف *str, PyObject* filename, صحیح منوال,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    اگر (PySys_Audit("compile", "yO", str, filename) < 0) {
        واپس تہی;
    }

    mod_ty نتیجہ = Py_Pegen_تسلسل_parser_from_string(str, منوال, filename, flags,
                                                    arena, module, incomplete);
    واپس نتیجہ;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                      صحیح منوال, ثابت حرف *ps1, ثابت حرف* ps2,
                      PyCompilerFlags *flags, صحیح *errcode, PyArena *arena)
{
    اگر (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        واپس تہی;
    }
    واپس Py_Pegen_تسلسل_parser_from_file_اشارہ_گر(fp, منوال, filename_ob, enc, ps1, ps2,
                                        flags, errcode, تہی, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                                 صحیح منوال, ثابت حرف *ps1, ثابت حرف* ps2,
                                 PyCompilerFlags *flags, صحیح *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    اگر (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        واپس تہی;
    }
    واپس Py_Pegen_تسلسل_parser_from_file_اشارہ_گر(fp, منوال, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
