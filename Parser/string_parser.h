#ifndef STRINGS_H
#define STRINGS_H

#অন্তর্ভাব <Python.h>
#অন্তর্ভাব <pycore_ast.h>
#অন্তর্ভাব "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, পূর্ণাঙ্ক, ধ্রুব বর্ণ *, size_t, Token *);

#endif
