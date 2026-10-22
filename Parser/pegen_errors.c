#अन्तर्भाव <Python.h>
#अन्तर्भाव <errcode.h>

#अन्तर्भाव "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#अन्तर्भाव "pycore_runtime.h"       // _Py_ID()
#अन्तर्भाव "pycore_tuple.h"         // _PyTuple_FromPair
#अन्तर्भाव "pegen.h"
#अन्तर्भाव "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

शून्य *
Py_Pegen_raise_incomplete_निवेश(Parser *p)
{
    यदि ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेश) && !PyErr_Occurred()) {
        प्रत्यागम _PyPegen_raise_error(p, Py_Exc_Incomplete_निवेश_Error, 0,
                                    "incomplete input");
    }
    प्रत्यागम रिक्त;
}

स्थिर अन्तःस्थ शून्य
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       पूर्णाङ्क error_lineno = info.delimiter_loc.lineno;
       पूर्णाङ्क error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

पूर्णाङ्क
_Pypegen_tokenizer_error(Parser *p)
{
    यदि (PyErr_Occurred()) {
        प्रत्यागम -1;
    }

    ध्रुव वर्ण *संदेश = रिक्त;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    यदि ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेश) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_निवेश(p);
        प्रत्यागम -1;
    }
    विकल्प (info.status) {
        पक्ष E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            प्रत्यागम -1;
        पक्ष E_TOKEN:
            संदेश = "invalid token";
            भङ्ग;
        पक्ष E_EOF:
            यदि (info.level) {
                raise_unclosed_parentheses_error(p);
            } अन्यथा {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            प्रत्यागम -1;
        पक्ष E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            प्रत्यागम -1;
        पक्ष E_INTR:
            यदि (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            प्रत्यागम -1;
        पक्ष E_NOMEM:
            PyErr_NoMemory();
            प्रत्यागम -1;
        पक्ष E_TABSPACE:
            errtype = PyExc_TabError;
            संदेश = "inconsistent use of tabs and spaces in indentation";
            भङ्ग;
        पक्ष E_TOODEEP:
            errtype = PyExc_IndentationError;
            संदेश = "too many levels of indentation";
            भङ्ग;
        पक्ष E_LINECONT: {
            col_offset = info.cursor - info.पंक्ति_span.start - 1;
            संदेश = "unexpected character after line continuation character";
            भङ्ग;
        }
        पक्ष E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            प्रत्यागम -1;
        स्वभाव:
            संदेश = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, संदेश);
    प्रत्यागम -1;
}

पूर्णाङ्क
_Pypegen_raise_decode_error(Parser *p)
{
    प्रतिज्ञा(PyErr_Occurred());
    ध्रुव वर्ण *errtype = रिक्त;
    यदि (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    अन्यथा यदि (PyErr_ExceptionMatches(Py_Exc_मूल्य_Error)) {
        errtype = "value error";
    }
    यदि (errtype) {
        PyObject *type;
        PyObject *मूल्य;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &मूल्य, &tback);
        errstr = PyObject_Str(मूल्य);
        यदि (errstr) {
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

    प्रत्यागम -1;
}

स्थिर पूर्णाङ्क
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    यदि (_PyTokenizer_IsInteractive(p->tok)) {
        प्रत्यागम 0;
    }

    PyObject *type, *मूल्य, *traceback;
    PyErr_Fetch(&type, &मूल्य, &traceback);

    Token *current_token = p->known_err_token != रिक्त ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_पंक्ति = current_token->lineno;

    पूर्णाङ्क ret = 0;
    संरचना token new_token;
    _PyToken_Init(&new_token);

    प्रति (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        विकल्प (new_token.type) {
            पक्ष ERRORTOKEN: {
                यदि (PyErr_Occurred()) {
                    ret = -1;
                    गच्छ निकास;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                यदि (info.level != 0) {
                    पूर्णाङ्क error_lineno = info.delimiter_loc.lineno;
                    यदि (current_err_पंक्ति > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        गच्छ निकास;
                    }
                }
                भङ्ग;
            }
            पक्ष ENDMARKER:
                भङ्ग;
            स्वभाव:
                अनुवर्त;
        }
        भङ्ग;
    }


निकास:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    यदि (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(मूल्य);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } अन्यथा {
        PyErr_Restore(type, मूल्य, traceback);
    }
    प्रत्यागम ret;
}

// PARSER ERRORS

शून्य *
_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णाङ्क use_mark, ध्रुव वर्ण *errmsg, ...)
{
    // Bail out if we already have an error set.
    यदि (p->error_indicator && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    यदि (p->fill == 0) {
        बहु_सूची va;
        बहु_शुरू(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        बहु_पूर्ण(va);
        प्रत्यागम रिक्त;
    }
    यदि (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    Token *t = p->known_err_token != रिक्त
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    यदि (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        यदि (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } अन्यथा यदि (info.cursor == info.निवेश_span.start) {
            col_offset = 0;
        } अन्यथा {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.पंक्ति_span.start, intptr_t, पूर्णाङ्क);
        }
    } अन्यथा {
        col_offset = t->col_offset + 1;
    }

    यदि (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    बहु_सूची va;
    बहु_शुरू(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    बहु_पूर्ण(va);

    प्रत्यागम रिक्त;
}

स्थिर PyObject *
get_error_पंक्ति_from_source(Parser *p, Py_ssize_t lineno)
{
    यदि (_PyTokenizer_RetainedSource(p->tok) == रिक्त) {
        प्रत्यागम Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ध्रुव वर्ण *पंक्ति = Py_Tokenizer_पंक्ति_View(p->tok, relative_lineno, &len);
    प्रत्यागम PyUnicode_DecodeUTF8(पंक्ति, len, "replace");
}

शून्य *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ध्रुव वर्ण *errmsg, बहु_सूची va)
{
    // Bail out if we already have an error set.
    यदि (p->error_indicator && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    PyObject *मूल्य = रिक्त;
    PyObject *errstr = रिक्त;
    PyObject *error_पंक्ति = रिक्त;
    PyObject *tmp = रिक्त;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.पंक्ति_span;

    यदि (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    यदि (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    यदि (!errstr) {
        गच्छ error;
    }

    यदि (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != रिक्त) {
        error_पंक्ति = get_error_पंक्ति_from_source(p, lineno);
    }
    अन्यथा यदि (p->start_rule == Py_file_निवेश) {
        error_पंक्ति = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (पूर्णाङ्क) lineno, info.encoding);
    }

    यदि (!error_पंक्ति) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        प्रतिज्ञा(!info.is_file || info.status == E_EOF);

        यदि (location.lineno <= lineno &&
                info.निवेश_span.end > info.निवेश_span.start) {
            Py_ssize_t size;
            ध्रुव वर्ण *पंक्ति = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            यदि (errtype == Py_Exc_Incomplete_निवेश_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_निवेश))) {
                size--;
            }
            error_पंक्ति = PyUnicode_DecodeUTF8(पंक्ति, size, "replace");
        }
        अन्यथा यदि (!info.is_file) {
            error_पंक्ति = get_error_पंक्ति_from_source(p, lineno);
        }
        अन्यथा {
            error_पंक्ति = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        यदि (!error_पंक्ति) {
            गच्छ error;
        }
    }

    Py_ssize_t col_संख्या = col_offset;
    Py_ssize_t end_col_संख्या = end_col_offset;

    col_संख्या = _PyPegen_byte_offset_to_character_offset(error_पंक्ति, col_offset);
    यदि (col_संख्या < 0) {
        गच्छ error;
    }

    यदि (end_col_offset > 0) {
        end_col_संख्या = _PyPegen_byte_offset_to_character_offset(error_पंक्ति, end_col_offset);
        यदि (end_col_संख्या < 0) {
            गच्छ error;
        }
    }

    tmp = Py_Build_मूल्य("(OnnNnn)", info.filename, lineno, col_संख्या, error_पंक्ति, end_lineno, end_col_संख्या);
    error_पंक्ति = रिक्त;
    यदि (!tmp) {
        गच्छ error;
    }
    मूल्य = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    यदि (!मूल्य) {
        गच्छ error;
    }
    PyErr_SetObject(errtype, मूल्य);

    Py_DECREF(errstr);
    Py_DECREF(मूल्य);
    प्रत्यागम रिक्त;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_पंक्ति);
    प्रत्यागम रिक्त;
}

शून्य
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    यदि (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        पूर्णाङ्क is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        यदि (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        प्रत्यागम;
    }
    // Initialization error
    यदि (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    यदि (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        यदि (info.level) {
            raise_unclosed_parentheses_error(p);
        } अन्यथा {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        प्रत्यागम;
    }
    // Indentation error in the tokenizer
    यदि (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        प्रत्यागम;
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
Pypegen_ढेर_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
