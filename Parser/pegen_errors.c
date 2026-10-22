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
    यदि ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) && !PyErr_Occurred()) {
        प्रत्यागम _PyPegen_raise_error(p, Py_Exc_Incomplete_निवेशः_Error, 0,
                                    "incomplete input");
    }
    प्रत्यागम रिक्त;
}

स्थिर अन्तःस्थ शून्य
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       पूर्णाङ्क error_lineno = info.delimiter_loc.lineno;
       पूर्णाङ्क error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_ज्ञातम्_LOCATION(p, PyExc_SyntaxError,
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

    ध्रुव वर्ण *सन्देशः = रिक्त;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_विस्थापनम् = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    यदि ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_निवेशः) &&
        (info.स्थितिः == E_EOF || info.स्थितिः == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_निवेशः(p);
        प्रत्यागम -1;
    }
    विकल्प (info.स्थितिः) {
        पक्ष E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            प्रत्यागम -1;
        पक्ष E_TOKEN:
            सन्देशः = "invalid token";
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
                Py_Err_समुच्चयः_None(Py_Exc_Keyboard_व्यवधानम्);
            }
            प्रत्यागम -1;
        पक्ष E_NOMEM:
            Py_Err_No_स्मृतिः();
            प्रत्यागम -1;
        पक्ष E_TABSPACE:
            errtype = PyExc_TabError;
            सन्देशः = "inconsistent use of tabs and spaces in indentation";
            भङ्ग;
        पक्ष E_TOODEEP:
            errtype = PyExc_IndentationError;
            सन्देशः = "too many levels of indentation";
            भङ्ग;
        पक्ष E_LINECONT: {
            col_विस्थापनम् = info.cursor - info.पङ्क्तिः_विस्तृतिः.start - 1;
            सन्देशः = "unexpected character after line continuation character";
            भङ्ग;
        }
        पक्ष E_COLUMNOVERFLOW:
            Py_Err_समुच्चयः_String(Py_Exc_अतिप्रवाहः_Error,
                    "Parser column offset overflow - source line is too big");
            प्रत्यागम -1;
        स्वभाव:
            सन्देशः = "unknown parsing error";
    }

    RAISE_ERROR_ज्ञातम्_LOCATION(p, errtype, info.location.lineno,
                               col_विस्थापनम् >= 0 ? col_विस्थापनम् : 0,
                               info.location.lineno, -1, सन्देशः);
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
    अन्यथा यदि (PyErr_ExceptionMatches(Py_Exc_मानम्_Error)) {
        errtype = "value error";
    }
    यदि (errtype) {
        PyObject *type;
        PyObject *मानम्;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &मानम्, &tback);
        errstr = PyObject_Str(मानम्);
        यदि (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        अन्यथा {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(मानम्);
        Py_XDECREF(tback);
    }

    प्रत्यागम -1;
}

स्थिर पूर्णाङ्क
Py_Pegen_tokenize_पूर्णम्_source_गन्तव्यम्_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    यदि (_PyTokenizer_IsInteractive(p->tok)) {
        प्रत्यागम 0;
    }

    PyObject *type, *मानम्, *traceback;
    PyErr_Fetch(&type, &मानम्, &traceback);

    Token *current_token = p->ज्ञातम्_err_token != रिक्त ? p->ज्ञातम्_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_पङ्क्तिः = current_token->lineno;

    पूर्णाङ्क ret = 0;
    संरचना token new_token;
    _PyToken_Init(&new_token);

    प्रति (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        विकल्प (new_token.type) {
            पक्ष ERRORTOKEN: {
                यदि (PyErr_Occurred()) {
                    ret = -1;
                    गच्छ निर्गच्छ;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                यदि (info.level != 0) {
                    पूर्णाङ्क error_lineno = info.delimiter_loc.lineno;
                    यदि (current_err_पङ्क्तिः > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        गच्छ निर्गच्छ;
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


निर्गच्छ:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    यदि (PyErr_Occurred() && !info.अन्तर्_formatted_string) {
        Py_XDECREF(मानम्);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } अन्यथा {
        PyErr_Restore(type, मानम्, traceback);
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
        va_list va;
        va_start(va, errmsg);
        Py_Pegen_raise_error_ज्ञातम्_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_अन्तः(va);
        प्रत्यागम रिक्त;
    }
    यदि (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        प्रत्यागम रिक्त;
    }
    Token *t = p->ज्ञातम्_err_token != रिक्त
                   ? p->ज्ञातम्_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_विस्थापनम्;
    Py_ssize_t अन्तः_col_विस्थापनम् = -1;
    यदि (t->col_विस्थापनम् == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        यदि (info.diagnostic.location.lineno != 0) {
            col_विस्थापनम् = info.diagnostic.location.byte_col;
        } अन्यथा यदि (info.cursor == info.निवेशः_विस्तृतिः.start) {
            col_विस्थापनम् = 0;
        } अन्यथा {
            col_विस्थापनम् = Py_SAFE_DOWNCAST(
                info.cursor - info.पङ्क्तिः_विस्तृतिः.start, intptr_t, पूर्णाङ्क);
        }
    } अन्यथा {
        col_विस्थापनम् = t->col_विस्थापनम् + 1;
    }

    यदि (t->अन्तः_col_विस्थापनम् != -1) {
        अन्तः_col_विस्थापनम् = t->अन्तः_col_विस्थापनम् + 1;
    }

    va_list va;
    va_start(va, errmsg);
    Py_Pegen_raise_error_ज्ञातम्_location(p, errtype, t->lineno, col_विस्थापनम्, t->अन्तः_lineno, अन्तः_col_विस्थापनम्, errmsg, va);
    va_अन्तः(va);

    प्रत्यागम रिक्त;
}

स्थिर PyObject *
get_error_पङ्क्तिः_from_source(Parser *p, Py_ssize_t lineno)
{
    यदि (_PyTokenizer_RetainedSource(p->tok) == रिक्त) {
        प्रत्यागम Py_GetConstant(Py_CONSTANT_रिक्तानि_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ध्रुव वर्ण *पङ्क्तिः = Py_Tokenizer_पङ्क्तिः_View(p->tok, relative_lineno, &len);
    प्रत्यागम PyUnicode_DecodeUTF8(पङ्क्तिः, len, "replace");
}

शून्य *
Py_Pegen_raise_error_ज्ञातम्_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_विस्थापनम्,
                                    Py_ssize_t अन्तः_lineno, Py_ssize_t अन्तः_col_विस्थापनम्,
                                    ध्रुव वर्ण *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    यदि (p->error_indicator && PyErr_Occurred()) {
        प्रत्यागम रिक्त;
    }
    PyObject *मानम् = रिक्त;
    PyObject *errstr = रिक्त;
    PyObject *error_पङ्क्तिः = रिक्त;
    PyObject *tmp = रिक्त;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_विस्तृतिः text_विस्तृतिः = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_विस्तृतिः : info.पङ्क्तिः_विस्तृतिः;

    यदि (अन्तः_lineno == CURRENT_POS) {
        अन्तः_lineno = location.lineno;
    }
    यदि (अन्तः_col_विस्थापनम् == CURRENT_POS) {
        अन्तः_col_विस्थापनम् = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    यदि (!errstr) {
        गच्छ error;
    }

    यदि (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != रिक्त) {
        error_पङ्क्तिः = get_error_पङ्क्तिः_from_source(p, lineno);
    }
    अन्यथा यदि (p->start_नियमः == Py_file_निवेशः) {
        error_पङ्क्तिः = Py_Err_क्रमादेशः_Decoded_Text_Object(info.filename,
                                                     (पूर्णाङ्क) lineno, info.encoding);
    }

    यदि (!error_पङ्क्तिः) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        प्रतिज्ञा(!info.is_file || info.स्थितिः == E_EOF);

        यदि (location.lineno <= lineno &&
                info.निवेशः_विस्तृतिः.अन्तः_2 > info.निवेशः_विस्तृतिः.start) {
            Py_ssize_t आकार;
            ध्रुव वर्ण *पङ्क्तिः = Py_Tokenizer_विस्तृतिः_View(
                p->tok, text_विस्तृतिः, &आकार);
            यदि (errtype == Py_Exc_Incomplete_निवेशः_Error && आकार > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_नियमः != Py_file_निवेशः))) {
                आकार--;
            }
            error_पङ्क्तिः = PyUnicode_DecodeUTF8(पङ्क्तिः, आकार, "replace");
        }
        अन्यथा यदि (!info.is_file) {
            error_पङ्क्तिः = get_error_पङ्क्तिः_from_source(p, lineno);
        }
        अन्यथा {
            error_पङ्क्तिः = Py_GetConstant(Py_CONSTANT_रिक्तानि_STR);
        }
        यदि (!error_पङ्क्तिः) {
            गच्छ error;
        }
    }

    Py_ssize_t col_सङ्ख्या = col_विस्थापनम्;
    Py_ssize_t अन्तः_col_सङ्ख्या = अन्तः_col_विस्थापनम्;

    col_सङ्ख्या = Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्(error_पङ्क्तिः, col_विस्थापनम्);
    यदि (col_सङ्ख्या < 0) {
        गच्छ error;
    }

    यदि (अन्तः_col_विस्थापनम् > 0) {
        अन्तः_col_सङ्ख्या = Py_Pegen_byte_विस्थापनम्_गन्तव्यम्_character_विस्थापनम्(error_पङ्क्तिः, अन्तः_col_विस्थापनम्);
        यदि (अन्तः_col_सङ्ख्या < 0) {
            गच्छ error;
        }
    }

    tmp = Py_Build_मानम्("(OnnNnn)", info.filename, lineno, col_सङ्ख्या, error_पङ्क्तिः, अन्तः_lineno, अन्तः_col_सङ्ख्या);
    error_पङ्क्तिः = रिक्त;
    यदि (!tmp) {
        गच्छ error;
    }
    मानम् = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    यदि (!मानम्) {
        गच्छ error;
    }
    Py_Err_समुच्चयः_Object(errtype, मानम्);

    Py_DECREF(errstr);
    Py_DECREF(मानम्);
    प्रत्यागम रिक्त;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_पङ्क्तिः);
    प्रत्यागम रिक्त;
}

शून्य
Pypegen_समुच्चयः_syntax_error(Parser* p, Token* चरमः_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    यदि (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        पूर्णाङ्क is_tok_ok = (info.स्थितिः == E_समाप्तम् || info.स्थितिः == E_OK);
        यदि (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_पूर्णम्_source_गन्तव्यम्_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        प्रत्यागम;
    }
    // Initialization error
    यदि (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    यदि (चरमः_token->type == ERRORTOKEN && info.स्थितिः == E_EOF) {
        यदि (info.level) {
            raise_unclosed_parentheses_error(p);
        } अन्यथा {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        प्रत्यागम;
    }
    // Indentation error in the tokenizer
    यदि (चरमः_token->type == INDENT || चरमः_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(चरमः_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        प्रत्यागम;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_ज्ञातम्_LOCATION(चरमः_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_पूर्णम्_source_गन्तव्यम्_check_for_errors(p);
}

शून्य
Pypegen_राशिः_अतिप्रवाहः(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_समुच्चयः_String(Py_Exc_स्मृतिः_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
