#अन्तर्भाव "Python.h"

#अन्तर्भाव "pegen.h"

mod_ty
_PyParser_ASTFromString(अचर वर्ण *str, PyObject* filename, पूर्णाङ्क mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    यदि (PySys_Audit("compile", "yO", str, filename) < 0) {
        फर्काऊ रिक्त;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    फर्काऊ result;
}

mod_ty
_PyParser_ASTFromFile(खाता *fp, PyObject *filename_ob, अचर वर्ण *enc,
                      पूर्णाङ्क mode, अचर वर्ण *ps1, अचर वर्ण* ps2,
                      PyCompilerFlags *flags, पूर्णाङ्क *errcode, PyArena *arena)
{
    यदि (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, रिक्त, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(खाता *fp, PyObject *filename_ob, अचर वर्ण *enc,
                                 पूर्णाङ्क mode, अचर वर्ण *ps1, अचर वर्ण* ps2,
                                 PyCompilerFlags *flags, पूर्णाङ्क *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    यदि (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        फर्काऊ रिक्त;
    }
    फर्काऊ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
