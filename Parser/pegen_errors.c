#شامل <Python.h>
#شامل <errcode.h>

#شامل "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#شامل "pycore_runtime.h"       // _Py_ID()
#شامل "pycore_tuple.h"         // _PyTuple_FromPair
#شامل "pegen.h"
#شامل "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

خالی *
Py_Pegen_raise_incomplete_داخلہ(Parser *p)
{
    اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) && !PyErr_Occurred()) {
        واپس _PyPegen_raise_error(p, Py_Exc_Incomplete_داخلہ_Error, 0,
                                    "incomplete input");
    }
    واپس تہی;
}

ساکن اندرونی خالی
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       صحیح error_lineno = info.delimiter_loc.lineno;
       صحیح error_ستون = info.delimiter_loc.byte_ستون;
       RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_ستون, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

صحیح
_Pypegen_tokenizer_error(Parser *p)
{
    اگر (PyErr_Occurred()) {
        واپس -1;
    }

    ثابت حرف *پیغام = تہی;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t ستون_انحراف = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلہ) &&
        (info.حالت_2 == E_EOF || info.حالت_2 == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_داخلہ(p);
        واپس -1;
    }
    انتخاب (info.حالت_2) {
        حالت E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            واپس -1;
        حالت E_TOKEN:
            پیغام = "invalid token";
            توڑو;
        حالت E_EOF:
            اگر (info.level) {
                raise_unclosed_parentheses_error(p);
            } ورنہ {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            واپس -1;
        حالت E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            واپس -1;
        حالت E_INTR:
            اگر (!PyErr_Occurred()) {
                Py_Err_سیٹ_None(Py_Exc_Keyboard_مداخلت);
            }
            واپس -1;
        حالت E_NOMEM:
            Py_Err_No_حافظہ();
            واپس -1;
        حالت E_TABSPACE:
            errtype = PyExc_TabError;
            پیغام = "inconsistent use of tabs and spaces in indentation";
            توڑو;
        حالت E_TOODEEP:
            errtype = PyExc_IndentationError;
            پیغام = "too many levels of indentation";
            توڑو;
        حالت E_LINECONT: {
            ستون_انحراف = info.cursor - info.سطر_وسعت.آغاز - 1;
            پیغام = "unexpected character after line continuation character";
            توڑو;
        }
        حالت E_COLUMNOVERFLOW:
            Py_Err_سیٹ_String(Py_Exc_حد_سے_تجاوز_Error,
                    "Parser column offset overflow - source line is too big");
            واپس -1;
        طےشدہ:
            پیغام = "unknown parsing error";
    }

    RAISE_ERROR_معلوم_LOCATION(p, errtype, info.location.lineno,
                               ستون_انحراف >= 0 ? ستون_انحراف : 0,
                               info.location.lineno, -1, پیغام);
    واپس -1;
}

صحیح
_Pypegen_raise_decode_error(Parser *p)
{
    أكد(PyErr_Occurred());
    ثابت حرف *errtype = تہی;
    اگر (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    ورنہ اگر (PyErr_ExceptionMatches(Py_Exc_قدر_Error)) {
        errtype = "value error";
    }
    اگر (errtype) {
        PyObject *type;
        PyObject *قدر;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &قدر, &tback);
        errstr = PyObject_Str(قدر);
        اگر (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        ورنہ {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(قدر);
        Py_XDECREF(tback);
    }

    واپس -1;
}

ساکن صحیح
Py_Pegen_tokenize_بھرا_ماخذ_مقصد_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    اگر (_PyTokenizer_IsInteractive(p->tok)) {
        واپس 0;
    }

    PyObject *type, *قدر, *traceback;
    PyErr_Fetch(&type, &قدر, &traceback);

    Token *current_token = p->معلوم_err_token != تہی ? p->معلوم_err_token : p->علامات[p->fill - 1];
    Py_ssize_t current_err_سطر = current_token->lineno;

    صحیح ret = 0;
    ساخت token new_token;
    _PyToken_Init(&new_token);

    برائے (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        انتخاب (new_token.type) {
            حالت ERRORTOKEN: {
                اگر (PyErr_Occurred()) {
                    ret = -1;
                    جاؤ باہر_نکلو;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                اگر (info.level != 0) {
                    صحیح error_lineno = info.delimiter_loc.lineno;
                    اگر (current_err_سطر > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        جاؤ باہر_نکلو;
                    }
                }
                توڑو;
            }
            حالت ENDMARKER:
                توڑو;
            طےشدہ:
                جاری;
        }
        توڑو;
    }


باہر_نکلو:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    اگر (PyErr_Occurred() && !info.اندر_formatted_string) {
        Py_XDECREF(قدر);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } ورنہ {
        PyErr_Restore(type, قدر, traceback);
    }
    واپس ret;
}

// PARSER ERRORS

خالی *
_PyPegen_raise_error(Parser *p, PyObject *errtype, صحیح use_mark, ثابت حرف *errmsg, ...)
{
    // Bail out if we already have an error set.
    اگر (p->error_indicator && PyErr_Occurred()) {
        واپس تہی;
    }
    اگر (p->fill == 0) {
        va_list va;
        va_آغاز(va, errmsg);
        Py_Pegen_raise_error_معلوم_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_انتہا(va);
        واپس تہی;
    }
    اگر (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        واپس تہی;
    }
    Token *t = p->معلوم_err_token != تہی
                   ? p->معلوم_err_token
                   : p->علامات[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t ستون_انحراف;
    Py_ssize_t انتہا_ستون_انحراف = -1;
    اگر (t->ستون_انحراف == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        اگر (info.diagnostic.location.lineno != 0) {
            ستون_انحراف = info.diagnostic.location.byte_ستون;
        } ورنہ اگر (info.cursor == info.داخلہ_وسعت.آغاز) {
            ستون_انحراف = 0;
        } ورنہ {
            ستون_انحراف = Py_محفوظ_DOWNCAST(
                info.cursor - info.سطر_وسعت.آغاز, intptr_t, صحیح);
        }
    } ورنہ {
        ستون_انحراف = t->ستون_انحراف + 1;
    }

    اگر (t->انتہا_ستون_انحراف != -1) {
        انتہا_ستون_انحراف = t->انتہا_ستون_انحراف + 1;
    }

    va_list va;
    va_آغاز(va, errmsg);
    Py_Pegen_raise_error_معلوم_location(p, errtype, t->lineno, ستون_انحراف, t->انتہا_lineno, انتہا_ستون_انحراف, errmsg, va);
    va_انتہا(va);

    واپس تہی;
}

ساکن PyObject *
get_error_سطر_from_ماخذ(Parser *p, Py_ssize_t lineno)
{
    اگر (Py_Tokenizer_Retained_ماخذ(p->tok) == تہی) {
        واپس Py_GetConstant(Py_CONSTANT_خالی_خانے_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *سطر = Py_Tokenizer_سطر_View(p->tok, relative_lineno, &len);
    واپس PyUnicode_DecodeUTF8(سطر, len, "replace");
}

خالی *
Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t ستون_انحراف,
                                    Py_ssize_t انتہا_lineno, Py_ssize_t انتہا_ستون_انحراف,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    اگر (p->error_indicator && PyErr_Occurred()) {
        واپس تہی;
    }
    PyObject *قدر = تہی;
    PyObject *errstr = تہی;
    PyObject *error_سطر = تہی;
    PyObject *tmp = تہی;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_وسعت متن_وسعت = info.diagnostic.location.lineno != 0
        ? info.diagnostic.متن_وسعت : info.سطر_وسعت;

    اگر (انتہا_lineno == CURRENT_POS) {
        انتہا_lineno = location.lineno;
    }
    اگر (انتہا_ستون_انحراف == CURRENT_POS) {
        انتہا_ستون_انحراف = location.byte_ستون;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    اگر (!errstr) {
        جاؤ error;
    }

    اگر (info.is_interactive && Py_Tokenizer_Retained_ماخذ(p->tok) != تہی) {
        error_سطر = get_error_سطر_from_ماخذ(p, lineno);
    }
    ورنہ اگر (p->آغاز_قاعدہ == Py_file_داخلہ) {
        error_سطر = Py_Err_پروگرام_Decoded_متن_Object(info.filename,
                                                     (صحیح) lineno, info.encoding);
    }

    اگر (!error_سطر) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.حالت_2 == E_EOF);

        اگر (location.lineno <= lineno &&
                info.داخلہ_وسعت.انتہا > info.داخلہ_وسعت.آغاز) {
            Py_ssize_t سائز;
            ثابت حرف *سطر = Py_Tokenizer_وسعت_View(
                p->tok, متن_وسعت, &سائز);
            اگر (errtype == Py_Exc_Incomplete_داخلہ_Error && سائز > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->آغاز_قاعدہ != Py_file_داخلہ))) {
                سائز--;
            }
            error_سطر = PyUnicode_DecodeUTF8(سطر, سائز, "replace");
        }
        ورنہ اگر (!info.is_file) {
            error_سطر = get_error_سطر_from_ماخذ(p, lineno);
        }
        ورنہ {
            error_سطر = Py_GetConstant(Py_CONSTANT_خالی_خانے_STR);
        }
        اگر (!error_سطر) {
            جاؤ error;
        }
    }

    Py_ssize_t ستون_نمبر = ستون_انحراف;
    Py_ssize_t انتہا_ستون_نمبر = انتہا_ستون_انحراف;

    ستون_نمبر = Py_Pegen_byte_انحراف_مقصد_character_انحراف(error_سطر, ستون_انحراف);
    اگر (ستون_نمبر < 0) {
        جاؤ error;
    }

    اگر (انتہا_ستون_انحراف > 0) {
        انتہا_ستون_نمبر = Py_Pegen_byte_انحراف_مقصد_character_انحراف(error_سطر, انتہا_ستون_انحراف);
        اگر (انتہا_ستون_نمبر < 0) {
            جاؤ error;
        }
    }

    tmp = Py_Build_قدر("(OnnNnn)", info.filename, lineno, ستون_نمبر, error_سطر, انتہا_lineno, انتہا_ستون_نمبر);
    error_سطر = تہی;
    اگر (!tmp) {
        جاؤ error;
    }
    قدر = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    اگر (!قدر) {
        جاؤ error;
    }
    Py_Err_سیٹ_Object(errtype, قدر);

    Py_DECREF(errstr);
    Py_DECREF(قدر);
    واپس تہی;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_سطر);
    واپس تہی;
}

خالی
Pypegen_سیٹ_syntax_error(Parser* p, Token* آخری_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    اگر (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحیح is_tok_ok = (info.حالت_2 == E_مکمل || info.حالت_2 == E_OK);
        اگر (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_بھرا_ماخذ_مقصد_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        واپس;
    }
    // Initialization error
    اگر (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    اگر (آخری_token->type == ERRORTOKEN && info.حالت_2 == E_EOF) {
        اگر (info.level) {
            raise_unclosed_parentheses_error(p);
        } ورنہ {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        واپس;
    }
    // Indentation error in the tokenizer
    اگر (آخری_token->type == INDENT || آخری_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(آخری_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        واپس;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_معلوم_LOCATION(آخری_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_بھرا_ماخذ_مقصد_check_for_errors(p);
}

خالی
Pypegen_ڈھیر_حد_سے_تجاوز(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_سیٹ_String(Py_Exc_حافظہ_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
