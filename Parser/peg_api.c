#شامل "Python.h"

#شامل "pegen.h"

mod_ty
_PyParser_ASTFromString(ثابت نویسه *str, PyObject* filename, صحیح mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    اگر (PySys_Audit("compile", "yO", str, filename) < 0) {
        بازگشت پوچ;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    بازگشت result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ثابت نویسه *enc,
                      صحیح mode, ثابت نویسه *ps1, ثابت نویسه* ps2,
                      PyCompilerFlags *flags, صحیح *errcode, PyArena *arena)
{
    اگر (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        بازگشت پوچ;
    }
    بازگشت _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, پوچ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ثابت نویسه *enc,
                                 صحیح mode, ثابت نویسه *ps1, ثابت نویسه* ps2,
                                 PyCompilerFlags *flags, صحیح *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    اگر (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        بازگشت پوچ;
    }
    بازگشت _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
