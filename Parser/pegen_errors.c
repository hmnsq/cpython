#ضمن <Python.h>
#ضمن <errcode.h>

#ضمن "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair
#ضمن "pegen.h"
#ضمن "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

فراغ *
Py_Pegen_raise_incomplete_داخلا(Parser *p)
{
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا) && !PyErr_Occurred()) {
        أرجع _PyPegen_raise_error(p, Py_Exc_Incomplete_داخلا_Error, 0,
                                    "incomplete input");
    }
    أرجع عدم;
}

ساكن مضمن فراغ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       صحيح error_lineno = info.delimiter_loc.lineno;
       صحيح error_کانو = info.delimiter_loc.byte_کانو;
       RAISE_ERROR_معلوم_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_کانو, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

صحيح
_Pypegen_tokenizer_error(Parser *p)
{
    إذا (PyErr_Occurred()) {
        أرجع -1;
    }

    ثابت حرف *نياپو = عدم;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t کانو_وٿي = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_داخلا) &&
        (info.حالت == E_EOF || info.حالت == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_داخلا(p);
        أرجع -1;
    }
    بدل (info.حالت) {
        حالة E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            أرجع -1;
        حالة E_TOKEN:
            نياپو = "invalid token";
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
                Py_Err_سيٽ_None(Py_Exc_Keyboard_مداخلت);
            }
            أرجع -1;
        حالة E_NOMEM:
            Py_Err_No_يادگيري();
            أرجع -1;
        حالة E_TABSPACE:
            errtype = PyExc_TabError;
            نياپو = "inconsistent use of tabs and spaces in indentation";
            اكسر;
        حالة E_TOODEEP:
            errtype = PyExc_IndentationError;
            نياپو = "too many levels of indentation";
            اكسر;
        حالة E_LINECONT: {
            کانو_وٿي = info.cursor - info.سٽ_وسعت.شروعات - 1;
            نياپو = "unexpected character after line continuation character";
            اكسر;
        }
        حالة E_COLUMNOVERFLOW:
            Py_Err_سيٽ_String(Py_Exc_حد_کان_مٿي_Error,
                    "Parser column offset overflow - source line is too big");
            أرجع -1;
        افتراضي:
            نياپو = "unknown parsing error";
    }

    RAISE_ERROR_معلوم_LOCATION(p, errtype, info.location.lineno,
                               کانو_وٿي >= 0 ? کانو_وٿي : 0,
                               info.location.lineno, -1, نياپو);
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
    وإلا إذا (PyErr_ExceptionMatches(Py_Exc_قدر_Error)) {
        errtype = "value error";
    }
    إذا (errtype) {
        PyObject *type;
        PyObject *قدر;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &قدر, &tback);
        errstr = PyObject_Str(قدر);
        إذا (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        وإلا {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(قدر);
        Py_XDECREF(tback);
    }

    أرجع -1;
}

ساكن صحيح
Py_Pegen_tokenize_ڀريل_ذريعو_منزل_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    إذا (_PyTokenizer_IsInteractive(p->tok)) {
        أرجع 0;
    }

    PyObject *type, *قدر, *traceback;
    PyErr_Fetch(&type, &قدر, &traceback);

    Token *current_token = p->معلوم_err_token != عدم ? p->معلوم_err_token : p->ٽوڪن[p->fill - 1];
    Py_ssize_t current_err_سٽ = current_token->lineno;

    صحيح ret = 0;
    بنية token new_token;
    _PyToken_Init(&new_token);

    لكل (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        بدل (new_token.type) {
            حالة ERRORTOKEN: {
                إذا (PyErr_Occurred()) {
                    ret = -1;
                    اذهب ٻاهر_نڪرو;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                إذا (info.level != 0) {
                    صحيح error_lineno = info.delimiter_loc.lineno;
                    إذا (current_err_سٽ > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        اذهب ٻاهر_نڪرو;
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


ٻاهر_نڪرو:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    إذا (PyErr_Occurred() && !info.اندريون_formatted_string) {
        Py_XDECREF(قدر);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } وإلا {
        PyErr_Restore(type, قدر, traceback);
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
        va_شروعات(va, errmsg);
        Py_Pegen_raise_error_معلوم_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_پڇاڙي(va);
        أرجع عدم;
    }
    إذا (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    Token *t = p->معلوم_err_token != عدم
                   ? p->معلوم_err_token
                   : p->ٽوڪن[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t کانو_وٿي;
    Py_ssize_t پڇاڙي_کانو_وٿي = -1;
    إذا (t->کانو_وٿي == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        إذا (info.diagnostic.location.lineno != 0) {
            کانو_وٿي = info.diagnostic.location.byte_کانو;
        } وإلا إذا (info.cursor == info.داخلا_وسعت.شروعات) {
            کانو_وٿي = 0;
        } وإلا {
            کانو_وٿي = Py_محفوظ_DOWNCAST(
                info.cursor - info.سٽ_وسعت.شروعات, intptr_t, صحيح);
        }
    } وإلا {
        کانو_وٿي = t->کانو_وٿي + 1;
    }

    إذا (t->پڇاڙي_کانو_وٿي != -1) {
        پڇاڙي_کانو_وٿي = t->پڇاڙي_کانو_وٿي + 1;
    }

    va_list va;
    va_شروعات(va, errmsg);
    Py_Pegen_raise_error_معلوم_location(p, errtype, t->lineno, کانو_وٿي, t->پڇاڙي_lineno, پڇاڙي_کانو_وٿي, errmsg, va);
    va_پڇاڙي(va);

    أرجع عدم;
}

ساكن PyObject *
get_error_سٽ_from_ذريعو(Parser *p, Py_ssize_t lineno)
{
    إذا (Py_Tokenizer_Retained_ذريعو(p->tok) == عدم) {
        أرجع Py_GetConstant(Py_CONSTANT_خالي_جايون_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *سٽ = Py_Tokenizer_سٽ_View(p->tok, relative_lineno, &len);
    أرجع PyUnicode_DecodeUTF8(سٽ, len, "replace");
}

فراغ *
Py_Pegen_raise_error_معلوم_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t کانو_وٿي,
                                    Py_ssize_t پڇاڙي_lineno, Py_ssize_t پڇاڙي_کانو_وٿي,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    إذا (p->error_indicator && PyErr_Occurred()) {
        أرجع عدم;
    }
    PyObject *قدر = عدم;
    PyObject *errstr = عدم;
    PyObject *error_سٽ = عدم;
    PyObject *tmp = عدم;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_وسعت متن_وسعت = info.diagnostic.location.lineno != 0
        ? info.diagnostic.متن_وسعت : info.سٽ_وسعت;

    إذا (پڇاڙي_lineno == CURRENT_POS) {
        پڇاڙي_lineno = location.lineno;
    }
    إذا (پڇاڙي_کانو_وٿي == CURRENT_POS) {
        پڇاڙي_کانو_وٿي = location.byte_کانو;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    إذا (!errstr) {
        اذهب error;
    }

    إذا (info.is_interactive && Py_Tokenizer_Retained_ذريعو(p->tok) != عدم) {
        error_سٽ = get_error_سٽ_from_ذريعو(p, lineno);
    }
    وإلا إذا (p->شروعات_قاعدو == Py_file_داخلا) {
        error_سٽ = Py_Err_پروگرام_Decoded_متن_Object(info.filename,
                                                     (صحيح) lineno, info.encoding);
    }

    إذا (!error_سٽ) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.حالت == E_EOF);

        إذا (location.lineno <= lineno &&
                info.داخلا_وسعت.پڇاڙي > info.داخلا_وسعت.شروعات) {
            Py_ssize_t سائيز;
            ثابت حرف *سٽ = Py_Tokenizer_وسعت_View(
                p->tok, متن_وسعت, &سائيز);
            إذا (errtype == Py_Exc_Incomplete_داخلا_Error && سائيز > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->شروعات_قاعدو != Py_file_داخلا))) {
                سائيز--;
            }
            error_سٽ = PyUnicode_DecodeUTF8(سٽ, سائيز, "replace");
        }
        وإلا إذا (!info.is_file) {
            error_سٽ = get_error_سٽ_from_ذريعو(p, lineno);
        }
        وإلا {
            error_سٽ = Py_GetConstant(Py_CONSTANT_خالي_جايون_STR);
        }
        إذا (!error_سٽ) {
            اذهب error;
        }
    }

    Py_ssize_t کانو_عدد = کانو_وٿي;
    Py_ssize_t پڇاڙي_کانو_عدد = پڇاڙي_کانو_وٿي;

    کانو_عدد = Py_Pegen_byte_وٿي_منزل_character_وٿي(error_سٽ, کانو_وٿي);
    إذا (کانو_عدد < 0) {
        اذهب error;
    }

    إذا (پڇاڙي_کانو_وٿي > 0) {
        پڇاڙي_کانو_عدد = Py_Pegen_byte_وٿي_منزل_character_وٿي(error_سٽ, پڇاڙي_کانو_وٿي);
        إذا (پڇاڙي_کانو_عدد < 0) {
            اذهب error;
        }
    }

    tmp = Py_Build_قدر("(OnnNnn)", info.filename, lineno, کانو_عدد, error_سٽ, پڇاڙي_lineno, پڇاڙي_کانو_عدد);
    error_سٽ = عدم;
    إذا (!tmp) {
        اذهب error;
    }
    قدر = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!قدر) {
        اذهب error;
    }
    Py_Err_سيٽ_Object(errtype, قدر);

    Py_DECREF(errstr);
    Py_DECREF(قدر);
    أرجع عدم;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_سٽ);
    أرجع عدم;
}

فراغ
Pypegen_سيٽ_syntax_error(Parser* p, Token* آخري_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    إذا (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحيح is_tok_ok = (info.حالت == E_پورو || info.حالت == E_OK);
        إذا (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_ڀريل_ذريعو_منزل_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        أرجع;
    }
    // Initialization error
    إذا (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    إذا (آخري_token->type == ERRORTOKEN && info.حالت == E_EOF) {
        إذا (info.level) {
            raise_unclosed_parentheses_error(p);
        } وإلا {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        أرجع;
    }
    // Indentation error in the tokenizer
    إذا (آخري_token->type == INDENT || آخري_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(آخري_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        أرجع;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_معلوم_LOCATION(آخري_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_ڀريل_ذريعو_منزل_check_for_errors(p);
}

فراغ
Pypegen_ڍير_حد_کان_مٿي(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_سيٽ_String(Py_Exc_يادگيري_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
