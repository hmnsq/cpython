#ಅನ್ತರ್ಭಾವ "Python.h"

#ಅನ್ತರ್ಭಾವ "pegen.h"

mod_ty
_PyParser_ASTFromString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, PyObject* filename, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ಆದರೆ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    mod_ty ಫಲಿತಾಂಶ = Py_Pegen_ಸತತ_parser_from_string(str, ಬಹುಲಕ, filename, flags,
                                                    arena, module, incomplete);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enc,
                      ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ps2,
                      PyCompilerFlags *flags, ಪೂರ್ಣಾಂಕ *errcode, PyArena *arena)
{
    ಆದರೆ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ Py_Pegen_ಸತತ_parser_from_file_ಸೂಚಕ(fp, ಬಹುಲಕ, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ಖಾಲಿ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enc,
                                 ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ps1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ps2,
                                 PyCompilerFlags *flags, ಪೂರ್ಣಾಂಕ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ಆದರೆ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ Py_Pegen_ಸತತ_parser_from_file_ಸೂಚಕ(fp, ಬಹುಲಕ, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
