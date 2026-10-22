#ifndef STRINGS_H
#define STRINGS_H

#অন্তর্ভাৱ <Python.h>
#অন্তর্ভাৱ <pycore_ast.h>
#অন্তর্ভাৱ "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, পূর্ণাঙ্ক, ধ্রুৱ ৱর্ণ *, size_t, Token *);

#endif
