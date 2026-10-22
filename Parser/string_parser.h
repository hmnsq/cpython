#ifndef STRINGS_H
#define STRINGS_H

#ضمن <Python.h>
#ضمن <pycore_ast.h>
#ضمن "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, صحيح, ثابت حرف *, size_t, Token *);

#endif
