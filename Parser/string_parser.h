#ifndef STRINGS_H
#define STRINGS_H

#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ <pycore_ast.h>
#અન્તર્ભાવ "pegen.h"

PyObject *_PyPegen_parse_string(Parser *, Token *);
PyObject *_PyPegen_decode_string(Parser *, પૂર્ણાંક, અચલ અક્ષર *, size_t, Token *);

#endif
