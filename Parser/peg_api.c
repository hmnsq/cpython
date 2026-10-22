#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "pegen.h"

mod_ty
_PyParser_ASTFromString(ಧ್ರುವ ವರ್ಣ *str, PyObject* filename, ಪೂರ್ಣಾಙ್ಕ mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ಯದಿ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ಪ್ರತ್ಯಾಗಮ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ಧ್ರುವ ವರ್ಣ *enc,
                      ಪೂರ್ಣಾಙ್ಕ mode, ಧ್ರುವ ವರ್ಣ *ps1, ಧ್ರುವ ವರ್ಣ* ps2,
                      PyCompilerFlags *flags, ಪೂರ್ಣಾಙ್ಕ *errcode, PyArena *arena)
{
    ಯದಿ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ರಿಕ್ತ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ಧ್ರುವ ವರ್ಣ *enc,
                                 ಪೂರ್ಣಾಙ್ಕ mode, ಧ್ರುವ ವರ್ಣ *ps1, ಧ್ರುವ ವರ್ಣ* ps2,
                                 PyCompilerFlags *flags, ಪೂರ್ಣಾಙ್ಕ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ಯದಿ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
