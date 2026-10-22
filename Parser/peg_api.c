#ضمن "Python.h"

#ضمن "pegen.h"

mod_ty
_PyParser_ASTFromString(ثابت حرف *str, PyObject* filename, صحيح mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    אם (PySys_Audit("compile", "yO", str, filename) < 0) {
        החזר عدم;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    החזר result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                      صحيح mode, ثابت حرف *ps1, ثابت حرف* ps2,
                      PyCompilerFlags *flags, صحيح *errcode, PyArena *arena)
{
    אם (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        החזר عدم;
    }
    החזר _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, عدم, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ثابت حرف *enc,
                                 صحيح mode, ثابت حرف *ps1, ثابت حرف* ps2,
                                 PyCompilerFlags *flags, صحيح *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    אם (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        החזר عدم;
    }
    החזר _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
