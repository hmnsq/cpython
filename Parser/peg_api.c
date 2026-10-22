#అన్తర్భావ "Python.h"

#అన్తర్భావ "pegen.h"

mod_ty
_PyParser_ASTFromString(స్థిరరాశి అక్షరం *str, PyObject* filename, పూర్ణాంకం బహుళకం,
                        PyCompilerFlags *flags, PyArena *arena,
                        PyObject *module, PyObject **incomplete)
{
    ఒకవేళ (PySys_Audit("compile", "yO", str, filename) < 0) {
        తిరిగి ఖాళీ;
    }

    mod_ty ఫలితం = Py_Pegen_నిరంతరం_parser_from_string(str, బహుళకం, filename, flags,
                                                    arena, module, incomplete);
    తిరిగి ఫలితం;
}

mod_ty
_PyParser_ASTFromFile(FILE *fp, PyObject *filename_ob, స్థిరరాశి అక్షరం *enc,
                      పూర్ణాంకం బహుళకం, స్థిరరాశి అక్షరం *ps1, స్థిరరాశి అక్షరం* ps2,
                      PyCompilerFlags *flags, పూర్ణాంకం *errcode, PyArena *arena)
{
    ఒకవేళ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        తిరిగి ఖాళీ;
    }
    తిరిగి Py_Pegen_నిరంతరం_parser_from_file_సూచిక(fp, బహుళకం, filename_ob, enc, ps1, ps2,
                                        flags, errcode, ఖాళీ, arena);
}

mod_ty
_PyParser_InteractiveASTFromFile(FILE *fp, PyObject *filename_ob, స్థిరరాశి అక్షరం *enc,
                                 పూర్ణాంకం బహుళకం, స్థిరరాశి అక్షరం *ps1, స్థిరరాశి అక్షరం* ps2,
                                 PyCompilerFlags *flags, పూర్ణాంకం *errcode,
                                 PyObject **interactive_src, PyArena *arena)
{
    ఒకవేళ (PySys_Audit("compile", "OO", Py_None, filename_ob) < 0) {
        తిరిగి ఖాళీ;
    }
    తిరిగి Py_Pegen_నిరంతరం_parser_from_file_సూచిక(fp, బహుళకం, filename_ob, enc, ps1, ps2,
                                                 flags, errcode, interactive_src, arena);
}
