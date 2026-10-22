#ifndef STRINGS_H
#define STRINGS_H

#شامل <Python.h>
#شامل <pycore_ast.h>
#شامل "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, صحیح, ثابت نویسه *, size_t, Token *);

#endif
