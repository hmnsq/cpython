#अन्तर्भाव <Python.h>
#अन्तर्भाव <errcode.h>

#अन्तर्भाव "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#अन्तर्भाव "pycore_runtime.h"       // _Py_ID()
#अन्तर्भाव "pycore_tuple.h"         // _PyTuple_FromPair
#अन्तर्भाव "pegen.h"
#अन्तर्भाव "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

शून्य *
Py_Pegen_raise_incomplete_निवेशः(Parser *p)
{
    सचे ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) && !PyErr_Occurred()) {
        पटिनिवत्त _PyPegen_raise_error(p, Py_Exc_Incomplete_निवेशः_Error, 0,
                                    "incomplete input");
    }
    पटिनिवत्त रिक्त;
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
    सचे (PyErr_Occurred()) {
        पटिनिवत्त -1;
    }

    ध्रुव वर्ण *सन्देशः = रिक्त;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    सचे ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_निवेशः(p);
        पटिनिवत्त -1;
    }
    विकल्प (info.status) {
        पक्ष E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            पटिनिवत्त -1;
        पक्ष E_TOKEN:
            सन्देशः = "invalid token";
            भङ्ग;
        पक्ष E_EOF:
            सचे (info.level) {
                raise_unclosed_parentheses_error(p);
            } अञ्ञथा {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            पटिनिवत्त -1;
        पक्ष E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            पटिनिवत्त -1;
        पक्ष E_INTR:
            सचे (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            पटिनिवत्त -1;
        पक्ष E_NOMEM:
            PyErr_NoMemory();
            पटिनिवत्त -1;
        पक्ष E_TABSPACE:
            errtype = PyExc_TabError;
            सन्देशः = "inconsistent use of tabs and spaces in indentation";
            भङ्ग;
        पक्ष E_TOODEEP:
            errtype = PyExc_IndentationError;
            सन्देशः = "too many levels of indentation";
            भङ्ग;
        पक्ष E_LINECONT: {
            col_offset = info.cursor - info.पङ्क्तिः_span.start - 1;
            सन्देशः = "unexpected character after line continuation character";
            भङ्ग;
        }
        पक्ष E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            पटिनिवत्त -1;
        स्वभाव:
            सन्देशः = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, सन्देशः);
    पटिनिवत्त -1;
}

पूर्णाङ्क
_Pypegen_raise_decode_error(Parser *p)
{
    प्रतिज्ञा(PyErr_Occurred());
    ध्रुव वर्ण *errtype = रिक्त;
    सचे (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    अञ्ञथा सचे (PyErr_ExceptionMatches(Py_Exc_मानम्_Error)) {
        errtype = "value error";
    }
    सचे (errtype) {
        PyObject *type;
        PyObject *मानम्;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &मानम्, &tback);
        errstr = PyObject_Str(मानम्);
        सचे (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        अञ्ञथा {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(मानम्);
        Py_XDECREF(tback);
    }

    पटिनिवत्त -1;
}

स्थिर पूर्णाङ्क
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    सचे (_PyTokenizer_IsInteractive(p->tok)) {
        पटिनिवत्त 0;
    }

    PyObject *type, *मानम्, *traceback;
    PyErr_Fetch(&type, &मानम्, &traceback);

    Token *current_token = p->known_err_token != रिक्त ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_पङ्क्तिः = current_token->lineno;

    पूर्णाङ्क ret = 0;
    संरचना token new_token;
    _PyToken_Init(&new_token);

    अत्थाय (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        विकल्प (new_token.type) {
            पक्ष ERRORTOKEN: {
                सचे (PyErr_Occurred()) {
                    ret = -1;
                    गच्छ exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                सचे (info.level != 0) {
                    पूर्णाङ्क error_lineno = info.delimiter_loc.lineno;
                    सचे (current_err_पङ्क्तिः > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        गच्छ exit;
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


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    सचे (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(मानम्);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } अञ्ञथा {
        PyErr_Restore(type, मानम्, traceback);
    }
    पटिनिवत्त ret;
}

// PARSER ERRORS

शून्य *
_PyPegen_raise_error(Parser *p, PyObject *errtype, पूर्णाङ्क use_mark, ध्रुव वर्ण *errmsg, ...)
{
    // Bail out if we already have an error set.
    सचे (p->error_indicator && PyErr_Occurred()) {
        पटिनिवत्त रिक्त;
    }
    सचे (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        पटिनिवत्त रिक्त;
    }
    सचे (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        पटिनिवत्त रिक्त;
    }
    Token *t = p->known_err_token != रिक्त
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    सचे (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        सचे (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } अञ्ञथा सचे (info.cursor == info.निवेशः_span.start) {
            col_offset = 0;
        } अञ्ञथा {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.पङ्क्तिः_span.start, intptr_t, पूर्णाङ्क);
        }
    } अञ्ञथा {
        col_offset = t->col_offset + 1;
    }

    सचे (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    पटिनिवत्त रिक्त;
}

स्थिर PyObject *
get_error_पङ्क्तिः_from_source(Parser *p, Py_ssize_t lineno)
{
    सचे (_PyTokenizer_RetainedSource(p->tok) == रिक्त) {
        पटिनिवत्त Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ध्रुव वर्ण *पङ्क्तिः = Py_Tokenizer_पङ्क्तिः_View(p->tok, relative_lineno, &len);
    पटिनिवत्त PyUnicode_DecodeUTF8(पङ्क्तिः, len, "replace");
}

शून्य *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ध्रुव वर्ण *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    सचे (p->error_indicator && PyErr_Occurred()) {
        पटिनिवत्त रिक्त;
    }
    PyObject *मानम् = रिक्त;
    PyObject *errstr = रिक्त;
    PyObject *error_पङ्क्तिः = रिक्त;
    PyObject *tmp = रिक्त;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.पङ्क्तिः_span;

    सचे (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    सचे (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    सचे (!errstr) {
        गच्छ error;
    }

    सचे (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != रिक्त) {
        error_पङ्क्तिः = get_error_पङ्क्तिः_from_source(p, lineno);
    }
    अञ्ञथा सचे (p->start_rule == Py_file_निवेशः) {
        error_पङ्क्तिः = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (पूर्णाङ्क) lineno, info.encoding);
    }

    सचे (!error_पङ्क्तिः) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        प्रतिज्ञा(!info.is_file || info.status == E_EOF);

        सचे (location.lineno <= lineno &&
                info.निवेशः_span.end > info.निवेशः_span.start) {
            Py_ssize_t size;
            ध्रुव वर्ण *पङ्क्तिः = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            सचे (errtype == Py_Exc_Incomplete_निवेशः_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_निवेशः))) {
                size--;
            }
            error_पङ्क्तिः = PyUnicode_DecodeUTF8(पङ्क्तिः, size, "replace");
        }
        अञ्ञथा सचे (!info.is_file) {
            error_पङ्क्तिः = get_error_पङ्क्तिः_from_source(p, lineno);
        }
        अञ्ञथा {
            error_पङ्क्तिः = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        सचे (!error_पङ्क्तिः) {
            गच्छ error;
        }
    }

    Py_ssize_t col_सङ्ख्या = col_offset;
    Py_ssize_t end_col_सङ्ख्या = end_col_offset;

    col_सङ्ख्या = _PyPegen_byte_offset_to_character_offset(error_पङ्क्तिः, col_offset);
    सचे (col_सङ्ख्या < 0) {
        गच्छ error;
    }

    सचे (end_col_offset > 0) {
        end_col_सङ्ख्या = _PyPegen_byte_offset_to_character_offset(error_पङ्क्तिः, end_col_offset);
        सचे (end_col_सङ्ख्या < 0) {
            गच्छ error;
        }
    }

    tmp = Py_Build_मानम्("(OnnNnn)", info.filename, lineno, col_सङ्ख्या, error_पङ्क्तिः, end_lineno, end_col_सङ्ख्या);
    error_पङ्क्तिः = रिक्त;
    सचे (!tmp) {
        गच्छ error;
    }
    मानम् = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    सचे (!मानम्) {
        गच्छ error;
    }
    PyErr_SetObject(errtype, मानम्);

    Py_DECREF(errstr);
    Py_DECREF(मानम्);
    पटिनिवत्त रिक्त;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_पङ्क्तिः);
    पटिनिवत्त रिक्त;
}

शून्य
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    सचे (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        पूर्णाङ्क is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        सचे (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        पटिनिवत्त;
    }
    // Initialization error
    सचे (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    सचे (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        सचे (info.level) {
            raise_unclosed_parentheses_error(p);
        } अञ्ञथा {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        पटिनिवत्त;
    }
    // Indentation error in the tokenizer
    सचे (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        पटिनिवत्त;
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
Pypegen_राशिः_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
