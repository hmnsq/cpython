#ضمن <Python.h>
#ضمن <errcode.h>

#ضمن "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair
#ضمن "pegen.h"
#ضمن "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

فراغ *
Py_Pegen_raise_incomplete_درون_داد(Parser *p)
{
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) && !PyErr_Occurred()) {
        أرجع _PyPegen_raise_error(p, Py_Exc_Incomplete_درون_داد_Error, 0,
                                    "incomplete input");
    }
    أرجع عدم;
}

ساكن مضمن فراغ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       صحيح error_lineno = info.delimiter_loc.lineno;
       صحيح error_ستون = info.delimiter_loc.byte_ستون;
       RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_ستون, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

صحيح
_Pypegen_tokenizer_error(Parser *p)
{
    إذا (PyErr_Occurred()) {
        أرجع -1;
    }

    ثابت حرف *پیام = عدم;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t ستون_جابه_جایی = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
        (info.وضعیت == E_EOF || info.وضعیت == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_درون_داد(p);
        أرجع -1;
    }
    بدل (info.وضعیت) {
        حالة E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            أرجع -1;
        حالة E_TOKEN:
            پیام = "invalid token";
            اكسر;
        حالة E_EOF:
            إذا (info.level) {
                raise_unclosed_parentheses_error(p);
            } وإلا {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            أرجع -1;
        حالة E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            أرجع -1;
        حالة E_INTR:
            إذا (!PyErr_Occurred()) {
                Py_Err_مجموعه_None(Py_Exc_Keyboard_وقفه);
            }
            أرجع -1;
        حالة E_NOMEM:
            Py_Err_No_حافظه();
            أرجع -1;
        حالة E_TABSPACE:
            errtype = PyExc_TabError;
            پیام = "inconsistent use of tabs and spaces in indentation";
            اكسر;
        حالة E_TOODEEP:
            errtype = PyExc_IndentationError;
            پیام = "too many levels of indentation";
            اكسر;
        حالة E_LINECONT: {
            ستون_جابه_جایی = info.cursor - info.سطر_بازه.شروع - 1;
            پیام = "unexpected character after line continuation character";
            اكسر;
        }
        حالة E_COLUMNOVERFLOW:
            Py_Err_مجموعه_String(Py_Exc_سرریز_Error,
                    "Parser column offset overflow - source line is too big");
            أرجع -1;
        افتراضي:
            پیام = "unknown parsing error";
    }

    RAISE_ERROR_معلوم_LOCATION(p, errtype, info.location.lineno,
                               ستون_جابه_جایی >= 0 ? ستون_جابه_جایی : 0,
                               info.location.lineno, -1, پیام);
    أرجع -1;
}

صحيح
_Pypegen_raise_decode_error(Parser *p)
{
    أكد(PyErr_Occurred());
    ثابت حرف *errtype = عدم;
    إذا (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    وإلا إذا (PyErr_ExceptionMatches(Py_Exc_مقدار_Error)) {
        errtype = "value error";
    }
    إذا (errtype) {
        PyObject *type;
        PyObject *مقدار;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &مقدار, &tback);
        errstr = PyObject_Str(مقدار);
        إذا (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        وإلا {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(مقدار);
        Py_XDECREF(tback);
    }

    أرجع -1;
}

ساكن صحيح
Py_Pegen_tokenize_پر_منبع_مقصد_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    إذا (_PyTokenizer_IsInteractive(p->tok)) {
        أرجع 0;
    }

    PyObject *type, *مقدار, *traceback;
    PyErr_Fetch(&type, &مقدار, &traceback);

    Token *current_token = p->معلوم_err_token != عدم ? p->معلوم_err_token : p->نشانهها[p->fill - 1];
    Py_ssize_t current_err_سطر = current_token->lineno;

    صحيح ret = 0;
    بنية token new_token;
    _PyToken_Init(&new_token);

    لكل (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        بدل (new_token.type) {
            حالة ERRORTOKEN: {
                إذا (PyErr_Occurred()) {
                    ret = -1;
                    اذهب خروج;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                إذا (info.level != 0) {
                    صحيح error_lineno = info.delimiter_loc.lineno;
                    إذا (current_err_سطر > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        اذهب خروج;
                    }
                }
                اكسر;
            }
            حالة ENDMARKER:
                اكسر;
            افتراضي:
                استمر;
        }
        اكسر;
    }


خروج:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    إذا (PyErr_Occurred() && !info.درون_formatted_string) {
        Py_XDECREF(مقدار);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } وإلا {
        PyErr_Restore(type, مقدار, traceback);
    }
    أرجع ret;
}

// PARSER ERRORS

فراغ *
_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...)
{
    // Bail out if we already have an error set.
    إذا (p->error_indicator && PyErr_Occurred()) {
        أرجع عدم;
    }
    إذا (p->fill == 0) {
        va_list va;
        va_شروع(va, errmsg);
        Py_Pegen_raise_error_معلوم_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_انتها(va);
        أرجع عدم;
    }
    إذا (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    Token *t = p->معلوم_err_token != عدم
                   ? p->معلوم_err_token
                   : p->نشانهها[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t ستون_جابه_جایی;
    Py_ssize_t انتها_ستون_جابه_جایی = -1;
    إذا (t->ستون_جابه_جایی == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        إذا (info.diagnostic.location.lineno != 0) {
            ستون_جابه_جایی = info.diagnostic.location.byte_ستون;
        } وإلا إذا (info.cursor == info.درون_داد_بازه.شروع) {
            ستون_جابه_جایی = 0;
        } وإلا {
            ستون_جابه_جایی = Py_امن_DOWNCAST(
                info.cursor - info.سطر_بازه.شروع, intptr_t, صحيح);
        }
    } وإلا {
        ستون_جابه_جایی = t->ستون_جابه_جایی + 1;
    }

    إذا (t->انتها_ستون_جابه_جایی != -1) {
        انتها_ستون_جابه_جایی = t->انتها_ستون_جابه_جایی + 1;
    }

    va_list va;
    va_شروع(va, errmsg);
    Py_Pegen_raise_error_معلوم_location(p, errtype, t->lineno, ستون_جابه_جایی, t->انتها_lineno, انتها_ستون_جابه_جایی, errmsg, va);
    va_انتها(va);

    أرجع عدم;
}

ساكن PyObject *
get_error_سطر_from_منبع(Parser *p, Py_ssize_t lineno)
{
    إذا (Py_Tokenizer_Retained_منبع(p->tok) == عدم) {
        أرجع Py_GetConstant(Py_CONSTANT_خالی_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *سطر = Py_Tokenizer_سطر_View(p->tok, relative_lineno, &len);
    أرجع PyUnicode_DecodeUTF8(سطر, len, "replace");
}

فراغ *
Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t ستون_جابه_جایی,
                                    Py_ssize_t انتها_lineno, Py_ssize_t انتها_ستون_جابه_جایی,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    إذا (p->error_indicator && PyErr_Occurred()) {
        أرجع عدم;
    }
    PyObject *مقدار = عدم;
    PyObject *errstr = عدم;
    PyObject *error_سطر = عدم;
    PyObject *tmp = عدم;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_بازه متن_بازه = info.diagnostic.location.lineno != 0
        ? info.diagnostic.متن_بازه : info.سطر_بازه;

    إذا (انتها_lineno == CURRENT_POS) {
        انتها_lineno = location.lineno;
    }
    إذا (انتها_ستون_جابه_جایی == CURRENT_POS) {
        انتها_ستون_جابه_جایی = location.byte_ستون;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    إذا (!errstr) {
        اذهب error;
    }

    إذا (info.is_interactive && Py_Tokenizer_Retained_منبع(p->tok) != عدم) {
        error_سطر = get_error_سطر_from_منبع(p, lineno);
    }
    وإلا إذا (p->شروع_قاعده == Py_file_درون_داد) {
        error_سطر = Py_Err_برنامه_Decoded_متن_Object(info.filename,
                                                     (صحيح) lineno, info.encoding);
    }

    إذا (!error_سطر) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.وضعیت == E_EOF);

        إذا (location.lineno <= lineno &&
                info.درون_داد_بازه.انتها > info.درون_داد_بازه.شروع) {
            Py_ssize_t اندازه;
            ثابت حرف *سطر = Py_Tokenizer_بازه_View(
                p->tok, متن_بازه, &اندازه);
            إذا (errtype == Py_Exc_Incomplete_درون_داد_Error && اندازه > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->شروع_قاعده != Py_file_درون_داد))) {
                اندازه--;
            }
            error_سطر = PyUnicode_DecodeUTF8(سطر, اندازه, "replace");
        }
        وإلا إذا (!info.is_file) {
            error_سطر = get_error_سطر_from_منبع(p, lineno);
        }
        وإلا {
            error_سطر = Py_GetConstant(Py_CONSTANT_خالی_STR);
        }
        إذا (!error_سطر) {
            اذهب error;
        }
    }

    Py_ssize_t ستون_عدد = ستون_جابه_جایی;
    Py_ssize_t انتها_ستون_عدد = انتها_ستون_جابه_جایی;

    ستون_عدد = Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی(error_سطر, ستون_جابه_جایی);
    إذا (ستون_عدد < 0) {
        اذهب error;
    }

    إذا (انتها_ستون_جابه_جایی > 0) {
        انتها_ستون_عدد = Py_Pegen_byte_جابه_جایی_مقصد_character_جابه_جایی(error_سطر, انتها_ستون_جابه_جایی);
        إذا (انتها_ستون_عدد < 0) {
            اذهب error;
        }
    }

    tmp = Py_Build_مقدار("(OnnNnn)", info.filename, lineno, ستون_عدد, error_سطر, انتها_lineno, انتها_ستون_عدد);
    error_سطر = عدم;
    إذا (!tmp) {
        اذهب error;
    }
    مقدار = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!مقدار) {
        اذهب error;
    }
    Py_Err_مجموعه_Object(errtype, مقدار);

    Py_DECREF(errstr);
    Py_DECREF(مقدار);
    أرجع عدم;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_سطر);
    أرجع عدم;
}

فراغ
Pypegen_مجموعه_syntax_error(Parser* p, Token* آخرین_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    إذا (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحيح is_tok_ok = (info.وضعیت == E_انجامشده || info.وضعیت == E_OK);
        إذا (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_پر_منبع_مقصد_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        أرجع;
    }
    // Initialization error
    إذا (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    إذا (آخرین_token->type == ERRORTOKEN && info.وضعیت == E_EOF) {
        إذا (info.level) {
            raise_unclosed_parentheses_error(p);
        } وإلا {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        أرجع;
    }
    // Indentation error in the tokenizer
    إذا (آخرین_token->type == INDENT || آخرین_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(آخرین_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        أرجع;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_معلوم_LOCATION(آخرین_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_پر_منبع_مقصد_check_for_errors(p);
}

فراغ
Pypegen_پشته_سرریز(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_مجموعه_String(Py_Exc_حافظه_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
