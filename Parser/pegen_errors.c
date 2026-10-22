#अन्तर्भाव <Python.h>
#अन्तर्भाव <errcode.h>

#अन्तर्भाव "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#अन्तर्भाव "pycore_runtime.h"       // _Py_ID()
#अन्तर्भाव "pycore_tuple.h"         // _PyTuple_FromPair
#अन्तर्भाव "pegen.h"
#अन्तर्भाव "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

शून्य *
Py_Pegen_raise_incomplete_आदान(Parser *p)
{
    जर ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_आदान) && !PyErr_Occurred()) {
        परत _PyPegen_raise_error(p, Py_Exc_Incomplete_आदान_Error, 0,
                                    "incomplete input");
    }
    परत रिक्त;
}

स्थिर ओळीत शून्य
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       पूर्णांक error_lineno = info.delimiter_loc.lineno;
       पूर्णांक error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

पूर्णांक
_Pypegen_tokenizer_error(Parser *p)
{
    जर (PyErr_Occurred()) {
        परत -1;
    }

    स्थिरांक अक्षर *संदेश = रिक्त;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    जर ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_आदान) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_आदान(p);
        परत -1;
    }
    निवड (info.status) {
        स्थिती E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            परत -1;
        स्थिती E_TOKEN:
            संदेश = "invalid token";
            थांबा;
        स्थिती E_EOF:
            जर (info.level) {
                raise_unclosed_parentheses_error(p);
            } अन्यथा {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            परत -1;
        स्थिती E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            परत -1;
        स्थिती E_INTR:
            जर (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            परत -1;
        स्थिती E_NOMEM:
            PyErr_NoMemory();
            परत -1;
        स्थिती E_TABSPACE:
            errtype = PyExc_TabError;
            संदेश = "inconsistent use of tabs and spaces in indentation";
            थांबा;
        स्थिती E_TOODEEP:
            errtype = PyExc_IndentationError;
            संदेश = "too many levels of indentation";
            थांबा;
        स्थिती E_LINECONT: {
            col_offset = info.cursor - info.ओळ_span.start - 1;
            संदेश = "unexpected character after line continuation character";
            थांबा;
        }
        स्थिती E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            परत -1;
        पूर्वनिर्धारित:
            संदेश = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, संदेश);
    परत -1;
}

पूर्णांक
_Pypegen_raise_decode_error(Parser *p)
{
    प्रतिज्ञा(PyErr_Occurred());
    स्थिरांक अक्षर *errtype = रिक्त;
    जर (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    अन्यथा जर (PyErr_ExceptionMatches(Py_Exc_मूल्य_Error)) {
        errtype = "value error";
    }
    जर (errtype) {
        PyObject *type;
        PyObject *मूल्य;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &मूल्य, &tback);
        errstr = PyObject_Str(मूल्य);
        जर (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        अन्यथा {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(मूल्य);
        Py_XDECREF(tback);
    }

    परत -1;
}

स्थिर पूर्णांक
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    जर (_PyTokenizer_IsInteractive(p->tok)) {
        परत 0;
    }

    PyObject *type, *मूल्य, *traceback;
    PyErr_Fetch(&type, &मूल्य, &traceback);

    Token *current_token = p->known_err_token != रिक्त ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_ओळ = current_token->lineno;

    पूर्णांक ret = 0;
    संरचना token new_token;
    _PyToken_Init(&new_token);

    साठी (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        निवड (new_token.type) {
            स्थिती ERRORTOKEN: {
                जर (PyErr_Occurred()) {
                    ret = -1;
                    जा निकास;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                जर (info.level != 0) {
                    पूर्णांक error_lineno = info.delimiter_loc.lineno;
                    जर (current_err_ओळ > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        जा निकास;
                    }
                }
                थांबा;
            }
            स्थिती ENDMARKER:
                थांबा;
            पूर्वनिर्धारित:
                सुरूठेवा;
        }
        थांबा;
    }


निकास:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    जर (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(मूल्य);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } अन्यथा {
        PyErr_Restore(type, मूल्य, traceback);
    }
    परत ret;
}

// PARSER ERRORS

शून्य *
_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णांक use_mark, स्थिरांक अक्षर *errmsg, ...)
{
    // Bail out if we already have an error set.
    जर (p->error_indicator && PyErr_Occurred()) {
        परत रिक्त;
    }
    जर (p->fill == 0) {
        बहु_सूची va;
        बहु_शुरू(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        बहु_पूर्ण(va);
        परत रिक्त;
    }
    जर (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        परत रिक्त;
    }
    Token *t = p->known_err_token != रिक्त
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    जर (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        जर (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } अन्यथा जर (info.cursor == info.आदान_span.start) {
            col_offset = 0;
        } अन्यथा {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.ओळ_span.start, intptr_t, पूर्णांक);
        }
    } अन्यथा {
        col_offset = t->col_offset + 1;
    }

    जर (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    बहु_सूची va;
    बहु_शुरू(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    बहु_पूर्ण(va);

    परत रिक्त;
}

स्थिर PyObject *
get_error_ओळ_from_source(Parser *p, Py_ssize_t lineno)
{
    जर (_PyTokenizer_RetainedSource(p->tok) == रिक्त) {
        परत Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    स्थिरांक अक्षर *ओळ = Py_Tokenizer_ओळ_View(p->tok, relative_lineno, &len);
    परत PyUnicode_DecodeUTF8(ओळ, len, "replace");
}

शून्य *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    स्थिरांक अक्षर *errmsg, बहु_सूची va)
{
    // Bail out if we already have an error set.
    जर (p->error_indicator && PyErr_Occurred()) {
        परत रिक्त;
    }
    PyObject *मूल्य = रिक्त;
    PyObject *errstr = रिक्त;
    PyObject *error_ओळ = रिक्त;
    PyObject *tmp = रिक्त;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.ओळ_span;

    जर (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    जर (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    जर (!errstr) {
        जा error;
    }

    जर (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != रिक्त) {
        error_ओळ = get_error_ओळ_from_source(p, lineno);
    }
    अन्यथा जर (p->start_rule == Py_file_आदान) {
        error_ओळ = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (पूर्णांक) lineno, info.encoding);
    }

    जर (!error_ओळ) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        प्रतिज्ञा(!info.is_file || info.status == E_EOF);

        जर (location.lineno <= lineno &&
                info.आदान_span.end > info.आदान_span.start) {
            Py_ssize_t size;
            स्थिरांक अक्षर *ओळ = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            जर (errtype == Py_Exc_Incomplete_आदान_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_आदान))) {
                size--;
            }
            error_ओळ = PyUnicode_DecodeUTF8(ओळ, size, "replace");
        }
        अन्यथा जर (!info.is_file) {
            error_ओळ = get_error_ओळ_from_source(p, lineno);
        }
        अन्यथा {
            error_ओळ = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        जर (!error_ओळ) {
            जा error;
        }
    }

    Py_ssize_t col_संख्या = col_offset;
    Py_ssize_t end_col_संख्या = end_col_offset;

    col_संख्या = _PyPegen_byte_offset_to_character_offset(error_ओळ, col_offset);
    जर (col_संख्या < 0) {
        जा error;
    }

    जर (end_col_offset > 0) {
        end_col_संख्या = _PyPegen_byte_offset_to_character_offset(error_ओळ, end_col_offset);
        जर (end_col_संख्या < 0) {
            जा error;
        }
    }

    tmp = Py_Build_मूल्य("(OnnNnn)", info.filename, lineno, col_संख्या, error_ओळ, end_lineno, end_col_संख्या);
    error_ओळ = रिक्त;
    जर (!tmp) {
        जा error;
    }
    मूल्य = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    जर (!मूल्य) {
        जा error;
    }
    PyErr_SetObject(errtype, मूल्य);

    Py_DECREF(errstr);
    Py_DECREF(मूल्य);
    परत रिक्त;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_ओळ);
    परत रिक्त;
}

शून्य
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    जर (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        पूर्णांक is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        जर (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        परत;
    }
    // Initialization error
    जर (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    जर (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        जर (info.level) {
            raise_unclosed_parentheses_error(p);
        } अन्यथा {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        परत;
    }
    // Indentation error in the tokenizer
    जर (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        परत;
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

शून्य
Pypegen_चळत_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
