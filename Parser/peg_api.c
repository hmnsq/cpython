#அந்தர்பாவ "Python.h"

#அந்தர்பாவ "pegen.h"

mod_ty
_PyParser_ASTFromString(மாறிலி எழுத்து *str, PyObject* filename, முழு mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    எனில் (PySys_Audit("compile", "yO", str, filename) < 0) {
        திரும்பு வெற்று;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    திரும்பு result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, மாறிலி எழுத்து *enc,
                      முழு mode, மாறிலி எழுத்து *ps1, மாறிலி எழுத்து* ps2,
                      PyCompilerFlags *flags, முழு *errcode, PyArena *arena)
{
    எனில் (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, வெற்று, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, மாறிலி எழுத்து *enc,
                                 முழு mode, மாறிலி எழுத்து *ps1, மாறிலி எழுத்து* ps2,
                                 PyCompilerFlags *flags, முழு *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    எனில் (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        திரும்பு வெற்று;
    }
    திரும்பு _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
