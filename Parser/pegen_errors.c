#അന്തര്ഭാവ <Python.h>
#അന്തര്ഭാവ <errcode.h>

#അന്തര്ഭാവ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#അന്തര്ഭാവ "pycore_runtime.h"       // _Py_ID()
#അന്തര്ഭാവ "pycore_tuple.h"         // _PyTuple_FromPair
#അന്തര്ഭാവ "pegen.h"
#അന്തര്ഭാവ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

ശൂന്യം *
Py_Pegen_raise_incomplete_നിവേശം(Parser *p)
{
    എങ്കിൽ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം) && !PyErr_Occurred()) {
        തിരികെ _PyPegen_raise_error(p, Py_Exc_Incomplete_നിവേശം_Error, 0,
                                    "incomplete input");
    }
    തിരികെ ശൂന്യ;
}

സ്ഥിതം വരിയിൽ ശൂന്യം
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       പൂർണ്ണസംഖ്യ error_lineno = info.delimiter_loc.lineno;
       പൂർണ്ണസംഖ്യ error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

പൂർണ്ണസംഖ്യ
_Pypegen_tokenizer_error(Parser *p)
{
    എങ്കിൽ (PyErr_Occurred()) {
        തിരികെ -1;
    }

    സ്ഥിരം അക്ഷരം *സന്ദേശം = ശൂന്യ;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    എങ്കിൽ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_നിവേശം) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_നിവേശം(p);
        തിരികെ -1;
    }
    തിരഞ്ഞെടുപ്പ് (info.status) {
        അവസ്ഥ E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            തിരികെ -1;
        അവസ്ഥ E_TOKEN:
            സന്ദേശം = "invalid token";
            നിർത്തുക;
        അവസ്ഥ E_EOF:
            എങ്കിൽ (info.level) {
                raise_unclosed_parentheses_error(p);
            } അല്ലെങ്കിൽ {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            തിരികെ -1;
        അവസ്ഥ E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            തിരികെ -1;
        അവസ്ഥ E_INTR:
            എങ്കിൽ (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            തിരികെ -1;
        അവസ്ഥ E_NOMEM:
            PyErr_NoMemory();
            തിരികെ -1;
        അവസ്ഥ E_TABSPACE:
            errtype = PyExc_TabError;
            സന്ദേശം = "inconsistent use of tabs and spaces in indentation";
            നിർത്തുക;
        അവസ്ഥ E_TOODEEP:
            errtype = PyExc_IndentationError;
            സന്ദേശം = "too many levels of indentation";
            നിർത്തുക;
        അവസ്ഥ E_LINECONT: {
            col_offset = info.cursor - info.വരി_span.start - 1;
            സന്ദേശം = "unexpected character after line continuation character";
            നിർത്തുക;
        }
        അവസ്ഥ E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            തിരികെ -1;
        സ്ഥിരസ്ഥിതി:
            സന്ദേശം = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, സന്ദേശം);
    തിരികെ -1;
}

പൂർണ്ണസംഖ്യ
_Pypegen_raise_decode_error(Parser *p)
{
    പ്രതിജ്ഞാ(PyErr_Occurred());
    സ്ഥിരം അക്ഷരം *errtype = ശൂന്യ;
    എങ്കിൽ (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PyErr_ExceptionMatches(Py_Exc_മൂല്യം_Error)) {
        errtype = "value error";
    }
    എങ്കിൽ (errtype) {
        PyObject *type;
        PyObject *മൂല്യം;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &മൂല്യം, &tback);
        errstr = PyObject_Str(മൂല്യം);
        എങ്കിൽ (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        അല്ലെങ്കിൽ {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(മൂല്യം);
        Py_XDECREF(tback);
    }

    തിരികെ -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    എങ്കിൽ (_PyTokenizer_IsInteractive(p->tok)) {
        തിരികെ 0;
    }

    PyObject *type, *മൂല്യം, *traceback;
    PyErr_Fetch(&type, &മൂല്യം, &traceback);

    Token *current_token = p->known_err_token != ശൂന്യ ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_വരി = current_token->lineno;

    പൂർണ്ണസംഖ്യ ret = 0;
    ഘടന token new_token;
    _PyToken_Init(&new_token);

    വേണ്ടി (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        തിരഞ്ഞെടുപ്പ് (new_token.type) {
            അവസ്ഥ ERRORTOKEN: {
                എങ്കിൽ (PyErr_Occurred()) {
                    ret = -1;
                    പോകുക exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                എങ്കിൽ (info.level != 0) {
                    പൂർണ്ണസംഖ്യ error_lineno = info.delimiter_loc.lineno;
                    എങ്കിൽ (current_err_വരി > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        പോകുക exit;
                    }
                }
                നിർത്തുക;
            }
            അവസ്ഥ ENDMARKER:
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                തുടരുക;
        }
        നിർത്തുക;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    എങ്കിൽ (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(മൂല്യം);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } അല്ലെങ്കിൽ {
        PyErr_Restore(type, മൂല്യം, traceback);
    }
    തിരികെ ret;
}

// PARSER ERRORS

ശൂന്യം *
_PyPegen_raise_error(Parser *p, PyObject *errtype, പൂർണ്ണസംഖ്യ use_mark, സ്ഥിരം അക്ഷരം *errmsg, ...)
{
    // Bail out if we already have an error set.
    എങ്കിൽ (p->error_indicator && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        തിരികെ ശൂന്യ;
    }
    Token *t = p->known_err_token != ശൂന്യ
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    എങ്കിൽ (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        എങ്കിൽ (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } അല്ലെങ്കിൽ എങ്കിൽ (info.cursor == info.നിവേശം_span.start) {
            col_offset = 0;
        } അല്ലെങ്കിൽ {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.വരി_span.start, intptr_t, പൂർണ്ണസംഖ്യ);
        }
    } അല്ലെങ്കിൽ {
        col_offset = t->col_offset + 1;
    }

    എങ്കിൽ (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    തിരികെ ശൂന്യ;
}

സ്ഥിതം PyObject *
get_error_വരി_from_source(Parser *p, Py_ssize_t lineno)
{
    എങ്കിൽ (_PyTokenizer_RetainedSource(p->tok) == ശൂന്യ) {
        തിരികെ Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    സ്ഥിരം അക്ഷരം *വരി = Py_Tokenizer_വരി_View(p->tok, relative_lineno, &len);
    തിരികെ PyUnicode_DecodeUTF8(വരി, len, "replace");
}

ശൂന്യം *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    സ്ഥിരം അക്ഷരം *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    എങ്കിൽ (p->error_indicator && PyErr_Occurred()) {
        തിരികെ ശൂന്യ;
    }
    PyObject *മൂല്യം = ശൂന്യ;
    PyObject *errstr = ശൂന്യ;
    PyObject *error_വരി = ശൂന്യ;
    PyObject *tmp = ശൂന്യ;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.വരി_span;

    എങ്കിൽ (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    എങ്കിൽ (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    എങ്കിൽ (!errstr) {
        പോകുക error;
    }

    എങ്കിൽ (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != ശൂന്യ) {
        error_വരി = get_error_വരി_from_source(p, lineno);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p->start_rule == Py_file_നിവേശം) {
        error_വരി = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (പൂർണ്ണസംഖ്യ) lineno, info.encoding);
    }

    എങ്കിൽ (!error_വരി) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        പ്രതിജ്ഞാ(!info.is_file || info.status == E_EOF);

        എങ്കിൽ (location.lineno <= lineno &&
                info.നിവേശം_span.end > info.നിവേശം_span.start) {
            Py_ssize_t size;
            സ്ഥിരം അക്ഷരം *വരി = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            എങ്കിൽ (errtype == Py_Exc_Incomplete_നിവേശം_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_നിവേശം))) {
                size--;
            }
            error_വരി = PyUnicode_DecodeUTF8(വരി, size, "replace");
        }
        അല്ലെങ്കിൽ എങ്കിൽ (!info.is_file) {
            error_വരി = get_error_വരി_from_source(p, lineno);
        }
        അല്ലെങ്കിൽ {
            error_വരി = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        എങ്കിൽ (!error_വരി) {
            പോകുക error;
        }
    }

    Py_ssize_t col_സംഖ്യ = col_offset;
    Py_ssize_t end_col_സംഖ്യ = end_col_offset;

    col_സംഖ്യ = _PyPegen_byte_offset_to_character_offset(error_വരി, col_offset);
    എങ്കിൽ (col_സംഖ്യ < 0) {
        പോകുക error;
    }

    എങ്കിൽ (end_col_offset > 0) {
        end_col_സംഖ്യ = _PyPegen_byte_offset_to_character_offset(error_വരി, end_col_offset);
        എങ്കിൽ (end_col_സംഖ്യ < 0) {
            പോകുക error;
        }
    }

    tmp = Py_Build_മൂല്യം("(OnnNnn)", info.filename, lineno, col_സംഖ്യ, error_വരി, end_lineno, end_col_സംഖ്യ);
    error_വരി = ശൂന്യ;
    എങ്കിൽ (!tmp) {
        പോകുക error;
    }
    മൂല്യം = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    എങ്കിൽ (!മൂല്യം) {
        പോകുക error;
    }
    PyErr_SetObject(errtype, മൂല്യം);

    Py_DECREF(errstr);
    Py_DECREF(മൂല്യം);
    തിരികെ ശൂന്യ;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_വരി);
    തിരികെ ശൂന്യ;
}

ശൂന്യം
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    എങ്കിൽ (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        പൂർണ്ണസംഖ്യ is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        എങ്കിൽ (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        തിരികെ;
    }
    // Initialization error
    എങ്കിൽ (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    എങ്കിൽ (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        എങ്കിൽ (info.level) {
            raise_unclosed_parentheses_error(p);
        } അല്ലെങ്കിൽ {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        തിരികെ;
    }
    // Indentation error in the tokenizer
    എങ്കിൽ (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        തിരികെ;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_KNOWN_LOCATION(last_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    _PyPegen_tokenize_full_source_to_check_for_errors(p);
}

ശൂന്യം
Pypegen_അട്ടി_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
