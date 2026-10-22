#ifndef STRINGS_H
#define STRINGS_H

#अन्तर्भाव <Python.h>
#अन्तर्भाव <pycore_ast.h>
#अन्तर्भाव "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, पूर्णांक, स्थिरांक अक्षर *, माप_प्रकार, Token *);

#endif
