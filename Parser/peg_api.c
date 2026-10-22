#अन्तर्भाव "Python.h"

#अन्तर्भाव "pegen.h"

mod_ty
_PyParser_ASTFromString(स्थिरांक अक्षर *str, PyObject* filename, पूर्णांक mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    जर (PySys_Audit("compile", "yO", str, filename) < 0) {
        परत रिक्त;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    परत result;
}

mod_ty
_PyParser_ASTFromFile(खाता *fp, PyObject *filename_ob, स्थिरांक अक्षर *enc,
                      पूर्णांक mode, स्थिरांक अक्षर *ps1, स्थिरांक अक्षर* ps2,
                      PyCompilerFlags *flags, पूर्णांक *errcode, PyArena *arena)
{
    जर (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        परत रिक्त;
    }
    परत _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, रिक्त, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(खाता *fp, PyObject *filename_ob, स्थिरांक अक्षर *enc,
                                 पूर्णांक mode, स्थिरांक अक्षर *ps1, स्थिरांक अक्षर* ps2,
                                 PyCompilerFlags *flags, पूर्णांक *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    जर (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        परत रिक्त;
    }
    परत _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
