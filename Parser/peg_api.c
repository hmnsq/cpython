#ଅନ୍ତର୍ଭାଵ "Python.h"

#ଅନ୍ତର୍ଭାଵ "pegen.h"

mod_ty
_PyParser_ASTFromString(ଅଚଳ ଅକ୍ଷର *str, PyObject* filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ଯଦି (PySys_Audit("compile", "yO", str, filename) < 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    mod_ty result = _PyPegen_run_parser_from_string(str, mode, filename, flags,
                                                    arena, module, incomplete);
    ଫେରନ୍ତୁ result;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, ଅଚଳ ଅକ୍ଷର *enc,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ଅଚଳ ଅକ୍ଷର *ps1, ଅଚଳ ଅକ୍ଷର* ps2,
                      PyCompilerFlags *flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errcode, PyArena *arena)
{
    ଯଦି (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ଖାଲି, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, ଅଚଳ ଅକ୍ଷର *enc,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ଅଚଳ ଅକ୍ଷର *ps1, ଅଚଳ ଅକ୍ଷର* ps2,
                                 PyCompilerFlags *flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ଯଦି (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ _PyPegen_run_parser_from_file_pointer(fp, mode, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
