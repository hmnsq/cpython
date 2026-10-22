#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "pegen.h"

mod_ty
_PyParser_ASTFromString(ਅਚਲ ਅੱਖਰ *str, PyObject* filename, ਪੂਰਨ ਬਹੁਲਕ,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ਜੇ (PySys_Audit("compile", "yO", str, filename) < 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    mod_ty ਨਤੀਜਾ = Py_Pegen_ਲੜੀਵਾਰ_parser_from_string(str, ਬਹੁਲਕ, filename, flags,
                                                    arena, module, incomplete);
    ਵਾਪਸ ਨਤੀਜਾ;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ਅਚਲ ਅੱਖਰ *enc,
                      ਪੂਰਨ ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ps1, ਅਚਲ ਅੱਖਰ* ps2,
                      PyCompilerFlags *flags, ਪੂਰਨ *errcode, PyArena *arena)
{
    ਜੇ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ Py_Pegen_ਲੜੀਵਾਰ_parser_from_file_ਸੂਚਕ(fp, ਬਹੁਲਕ, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ਨਿਰਾ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ਅਚਲ ਅੱਖਰ *enc,
                                 ਪੂਰਨ ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ps1, ਅਚਲ ਅੱਖਰ* ps2,
                                 PyCompilerFlags *flags, ਪੂਰਨ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ਜੇ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ Py_Pegen_ਲੜੀਵਾਰ_parser_from_file_ਸੂਚਕ(fp, ਬਹੁਲਕ, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
