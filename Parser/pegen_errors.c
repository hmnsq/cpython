#ضمن <Python.h>
#ضمن <errcode.h>

#ضمن "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair
#ضمن "pegen.h"
#ضمن "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

فراغ *
Py_Pegen_raise_incomplete_ننوت(Parser *p)
{
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت) && !PyErr_Occurred()) {
        أرجع _PyPegen_raise_error(p, Py_Exc_Incomplete_ننوت_Error, 0,
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

    ثابت حرف *پیغام = عدم;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t ستون_دننه_واټن = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ننوت) &&
        (info.حالت == E_EOF || info.حالت == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_ننوت(p);
        أرجع -1;
    }
    بدل (info.حالت) {
        حالة E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            أرجع -1;
        حالة E_TOKEN:
            پیغام = "invalid token";
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
                Py_Err_سېټ_None(Py_Exc_Keyboard_مداخله);
            }
            أرجع -1;
        حالة E_NOMEM:
            Py_Err_No_حافظه();
            أرجع -1;
        حالة E_TABSPACE:
            errtype = PyExc_TabError;
            پیغام = "inconsistent use of tabs and spaces in indentation";
            اكسر;
        حالة E_TOODEEP:
            errtype = PyExc_IndentationError;
            پیغام = "too many levels of indentation";
            اكسر;
        حالة E_LINECONT: {
            ستون_دننه_واټن = info.cursor - info.کرښه_پراختیا.پیل - 1;
            پیغام = "unexpected character after line continuation character";
            اكسر;
        }
        حالة E_COLUMNOVERFLOW:
            Py_Err_سېټ_String(Py_Exc_ډکېدنه_Error,
                    "Parser column offset overflow - source line is too big");
            أرجع -1;
        افتراضي:
            پیغام = "unknown parsing error";
    }

    RAISE_ERROR_معلوم_LOCATION(p, errtype, info.location.lineno,
                               ستون_دننه_واټن >= 0 ? ستون_دننه_واټن : 0,
                               info.location.lineno, -1, پیغام);
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
    وإلا إذا (PyErr_ExceptionMatches(Py_Exc_ارزښت_Error)) {
        errtype = "value error";
    }
    إذا (errtype) {
        PyObject *type;
        PyObject *ارزښت;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &ارزښت, &tback);
        errstr = PyObject_Str(ارزښت);
        إذا (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        وإلا {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(ارزښت);
        Py_XDECREF(tback);
    }

    أرجع -1;
}

ساكن صحيح
Py_Pegen_tokenize_ډک_سرچینه_منزل_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    إذا (_PyTokenizer_IsInteractive(p->tok)) {
        أرجع 0;
    }

    PyObject *type, *ارزښت, *traceback;
    PyErr_Fetch(&type, &ارزښت, &traceback);

    Token *current_token = p->معلوم_err_token != عدم ? p->معلوم_err_token : p->نښې[p->fill - 1];
    Py_ssize_t current_err_کرښه = current_token->lineno;

    صحيح ret = 0;
    بنية token new_token;
    _PyToken_Init(&new_token);

    لكل (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        بدل (new_token.type) {
            حالة ERRORTOKEN: {
                إذا (PyErr_Occurred()) {
                    ret = -1;
                    اذهب وتل;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                إذا (info.level != 0) {
                    صحيح error_lineno = info.delimiter_loc.lineno;
                    إذا (current_err_کرښه > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        اذهب وتل;
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


وتل:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    إذا (PyErr_Occurred() && !info.دننه_formatted_string) {
        Py_XDECREF(ارزښت);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } وإلا {
        PyErr_Restore(type, ارزښت, traceback);
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
        va_پیل(va, errmsg);
        Py_Pegen_raise_error_معلوم_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_پای_ټکی(va);
        أرجع عدم;
    }
    إذا (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    Token *t = p->معلوم_err_token != عدم
                   ? p->معلوم_err_token
                   : p->نښې[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t ستون_دننه_واټن;
    Py_ssize_t پای_ټکی_ستون_دننه_واټن = -1;
    إذا (t->ستون_دننه_واټن == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        إذا (info.diagnostic.location.lineno != 0) {
            ستون_دننه_واټن = info.diagnostic.location.byte_ستون;
        } وإلا إذا (info.cursor == info.ننوت_پراختیا.پیل) {
            ستون_دننه_واټن = 0;
        } وإلا {
            ستون_دننه_واټن = Py_خوندي_DOWNCAST(
                info.cursor - info.کرښه_پراختیا.پیل, intptr_t, صحيح);
        }
    } وإلا {
        ستون_دننه_واټن = t->ستون_دننه_واټن + 1;
    }

    إذا (t->پای_ټکی_ستون_دننه_واټن != -1) {
        پای_ټکی_ستون_دننه_واټن = t->پای_ټکی_ستون_دننه_واټن + 1;
    }

    va_list va;
    va_پیل(va, errmsg);
    Py_Pegen_raise_error_معلوم_location(p, errtype, t->lineno, ستون_دننه_واټن, t->پای_ټکی_lineno, پای_ټکی_ستون_دننه_واټن, errmsg, va);
    va_پای_ټکی(va);

    أرجع عدم;
}

ساكن PyObject *
get_error_کرښه_from_سرچینه(Parser *p, Py_ssize_t lineno)
{
    إذا (Py_Tokenizer_Retained_سرچینه(p->tok) == عدم) {
        أرجع Py_GetConstant(Py_CONSTANT_تشه_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *کرښه = Py_Tokenizer_کرښه_View(p->tok, relative_lineno, &len);
    أرجع PyUnicode_DecodeUTF8(کرښه, len, "replace");
}

فراغ *
Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t ستون_دننه_واټن,
                                    Py_ssize_t پای_ټکی_lineno, Py_ssize_t پای_ټکی_ستون_دننه_واټن,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    إذا (p->error_indicator && PyErr_Occurred()) {
        أرجع عدم;
    }
    PyObject *ارزښت = عدم;
    PyObject *errstr = عدم;
    PyObject *error_کرښه = عدم;
    PyObject *tmp = عدم;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_پراختیا متن_پراختیا = info.diagnostic.location.lineno != 0
        ? info.diagnostic.متن_پراختیا : info.کرښه_پراختیا;

    إذا (پای_ټکی_lineno == CURRENT_POS) {
        پای_ټکی_lineno = location.lineno;
    }
    إذا (پای_ټکی_ستون_دننه_واټن == CURRENT_POS) {
        پای_ټکی_ستون_دننه_واټن = location.byte_ستون;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    إذا (!errstr) {
        اذهب error;
    }

    إذا (info.is_interactive && Py_Tokenizer_Retained_سرچینه(p->tok) != عدم) {
        error_کرښه = get_error_کرښه_from_سرچینه(p, lineno);
    }
    وإلا إذا (p->پیل_قاعده == Py_file_ننوت) {
        error_کرښه = Py_Err_پروګرام_Decoded_متن_Object(info.filename,
                                                     (صحيح) lineno, info.encoding);
    }

    إذا (!error_کرښه) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.حالت == E_EOF);

        إذا (location.lineno <= lineno &&
                info.ننوت_پراختیا.پای_ټکی > info.ننوت_پراختیا.پیل) {
            Py_ssize_t اندازه;
            ثابت حرف *کرښه = Py_Tokenizer_پراختیا_View(
                p->tok, متن_پراختیا, &اندازه);
            إذا (errtype == Py_Exc_Incomplete_ننوت_Error && اندازه > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->پیل_قاعده != Py_file_ننوت))) {
                اندازه--;
            }
            error_کرښه = PyUnicode_DecodeUTF8(کرښه, اندازه, "replace");
        }
        وإلا إذا (!info.is_file) {
            error_کرښه = get_error_کرښه_from_سرچینه(p, lineno);
        }
        وإلا {
            error_کرښه = Py_GetConstant(Py_CONSTANT_تشه_STR);
        }
        إذا (!error_کرښه) {
            اذهب error;
        }
    }

    Py_ssize_t ستون_شمېره = ستون_دننه_واټن;
    Py_ssize_t پای_ټکی_ستون_شمېره = پای_ټکی_ستون_دننه_واټن;

    ستون_شمېره = Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن(error_کرښه, ستون_دننه_واټن);
    إذا (ستون_شمېره < 0) {
        اذهب error;
    }

    إذا (پای_ټکی_ستون_دننه_واټن > 0) {
        پای_ټکی_ستون_شمېره = Py_Pegen_byte_دننه_واټن_منزل_character_دننه_واټن(error_کرښه, پای_ټکی_ستون_دننه_واټن);
        إذا (پای_ټکی_ستون_شمېره < 0) {
            اذهب error;
        }
    }

    tmp = Py_Build_ارزښت("(OnnNnn)", info.filename, lineno, ستون_شمېره, error_کرښه, پای_ټکی_lineno, پای_ټکی_ستون_شمېره);
    error_کرښه = عدم;
    إذا (!tmp) {
        اذهب error;
    }
    ارزښت = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!ارزښت) {
        اذهب error;
    }
    Py_Err_سېټ_Object(errtype, ارزښت);

    Py_DECREF(errstr);
    Py_DECREF(ارزښت);
    أرجع عدم;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_کرښه);
    أرجع عدم;
}

فراغ
Pypegen_سېټ_syntax_error(Parser* p, Token* وروستی_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    إذا (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحيح is_tok_ok = (info.حالت == E_بشپړ || info.حالت == E_OK);
        إذا (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_ډک_سرچینه_منزل_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        أرجع;
    }
    // Initialization error
    إذا (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    إذا (وروستی_token->type == ERRORTOKEN && info.حالت == E_EOF) {
        إذا (info.level) {
            raise_unclosed_parentheses_error(p);
        } وإلا {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        أرجع;
    }
    // Indentation error in the tokenizer
    إذا (وروستی_token->type == INDENT || وروستی_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(وروستی_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        أرجع;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_معلوم_LOCATION(وروستی_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_ډک_سرچینه_منزل_check_for_errors(p);
}

فراغ
Pypegen_ډېری_ډکېدنه(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_سېټ_String(Py_Exc_حافظه_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
