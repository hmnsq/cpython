#ifndef STRINGS_H
#define STRINGS_H

#अन्तर्भाव <Python.h>
#अन्तर्भाव <pycore_ast.h>
#अन्तर्भाव "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, पूर्णाङ्क, अचर वर्ण *, माप_प्रकार, Token *);

#endif
