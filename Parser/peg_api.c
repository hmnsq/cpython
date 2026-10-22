#അന്തര്ഭാവ "Python.h"

#അന്തര്ഭാവ "pegen.h"

mod_ty
_PyParser_ASTFromString(സ്ഥിരം അക്ഷരം *str, PyObject* filename, പൂർണ്ണസംഖ്യ mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    എങ്കിൽ (PySys_Audit("compile", "yO", str, filename) < 0) {
        തിരികെ ശൂന്യ;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    തിരികെ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, സ്ഥിരം അക്ഷരം *enc,
                      പൂർണ്ണസംഖ്യ mode, സ്ഥിരം അക്ഷരം *ps1, സ്ഥിരം അക്ഷരം* ps2,
                      PyCompilerFlags *flags, പൂർണ്ണസംഖ്യ *errcode, PyArena *arena)
{
    എങ്കിൽ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ശൂന്യ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, സ്ഥിരം അക്ഷരം *enc,
                                 പൂർണ്ണസംഖ്യ mode, സ്ഥിരം അക്ഷരം *ps1, സ്ഥിരം അക്ഷരം* ps2,
                                 PyCompilerFlags *flags, പൂർണ്ണസംഖ്യ *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    എങ്കിൽ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        തിരികെ ശൂന്യ;
    }
    തിരികെ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
