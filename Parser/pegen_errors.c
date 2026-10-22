#ਅਨ੍ਤਰ੍ਭਾਵ <Python.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errcode.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_runtime.h"       // _Py_ID()
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_tuple.h"         // _PyTuple_FromPair
#ਅਨ੍ਤਰ੍ਭਾਵ "pegen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

ਖਾਲੀ *
Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(Parser *p)
{
    ਜੇ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) && !PyErr_Occurred()) {
        ਵਾਪਸ _PyPegen_raise_error(p, Py_Exc_Incomplete_ਨਿਵੇਸ਼_Error, 0,
                                    "incomplete input");
    }
    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       ਪੂਰਨ error_lineno = info.delimiter_loc.lineno;
       ਪੂਰਨ error_ਖਾਨਾ = info.delimiter_loc.byte_ਖਾਨਾ;
       RAISE_ERROR_ਗਿਆਤ_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_ਖਾਨਾ, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

ਪੂਰਨ
_Pypegen_tokenizer_error(Parser *p)
{
    ਜੇ (PyErr_Occurred()) {
        ਵਾਪਸ -1;
    }

    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = ਨਿਰਾ;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t ਖਾਨਾ_ਵਿੱਥ = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ਜੇ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ਨਿਵੇਸ਼) &&
        (info.ਸਥਿਤੀ == E_EOF || info.ਸਥਿਤੀ == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_ਨਿਵੇਸ਼(p);
        ਵਾਪਸ -1;
    }
    ਚੋਣ (info.ਸਥਿਤੀ) {
        ਹਾਲਤ E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            ਵਾਪਸ -1;
        ਹਾਲਤ E_TOKEN:
            ਸੁਨੇਹਾ = "invalid token";
            ਰੋਕੋ;
        ਹਾਲਤ E_EOF:
            ਜੇ (info.level) {
                raise_unclosed_parentheses_error(p);
            } ਨਹੀਂਤਾਂ {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            ਵਾਪਸ -1;
        ਹਾਲਤ E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            ਵਾਪਸ -1;
        ਹਾਲਤ E_INTR:
            ਜੇ (!PyErr_Occurred()) {
                Py_Err_ਸੈੱਟ_None(Py_Exc_Keyboard_ਰੁਕਾਵਟ);
            }
            ਵਾਪਸ -1;
        ਹਾਲਤ E_NOMEM:
            Py_Err_No_ਸਮ੍ਰਿਤੀ();
            ਵਾਪਸ -1;
        ਹਾਲਤ E_TABSPACE:
            errtype = PyExc_TabError;
            ਸੁਨੇਹਾ = "inconsistent use of tabs and spaces in indentation";
            ਰੋਕੋ;
        ਹਾਲਤ E_TOODEEP:
            errtype = PyExc_IndentationError;
            ਸੁਨੇਹਾ = "too many levels of indentation";
            ਰੋਕੋ;
        ਹਾਲਤ E_LINECONT: {
            ਖਾਨਾ_ਵਿੱਥ = info.cursor - info.ਸਤਰ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ - 1;
            ਸੁਨੇਹਾ = "unexpected character after line continuation character";
            ਰੋਕੋ;
        }
        ਹਾਲਤ E_COLUMNOVERFLOW:
            Py_Err_ਸੈੱਟ_String(Py_Exc_ਉਛਾਲ_Error,
                    "Parser column offset overflow - source line is too big");
            ਵਾਪਸ -1;
        ਮੂਲ:
            ਸੁਨੇਹਾ = "unknown parsing error";
    }

    RAISE_ERROR_ਗਿਆਤ_LOCATION(p, errtype, info.location.lineno,
                               ਖਾਨਾ_ਵਿੱਥ >= 0 ? ਖਾਨਾ_ਵਿੱਥ : 0,
                               info.location.lineno, -1, ਸੁਨੇਹਾ);
    ਵਾਪਸ -1;
}

ਪੂਰਨ
_Pypegen_raise_decode_error(Parser *p)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(PyErr_Occurred());
    ਅਚਲ ਅੱਖਰ *errtype = ਨਿਰਾ;
    ਜੇ (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    ਨਹੀਂਤਾਂ ਜੇ (PyErr_ExceptionMatches(Py_Exc_ਮੁੱਲ_Error)) {
        errtype = "value error";
    }
    ਜੇ (errtype) {
        PyObject *type;
        PyObject *ਮੁੱਲ;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &ਮੁੱਲ, &tback);
        errstr = PyObject_Str(ਮੁੱਲ);
        ਜੇ (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        ਨਹੀਂਤਾਂ {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(ਮੁੱਲ);
        Py_XDECREF(tback);
    }

    ਵਾਪਸ -1;
}

ਸਥਿਰ ਪੂਰਨ
Py_Pegen_tokenize_ਭਰਿਆ_ਸਰੋਤ_ਮੰਜ਼ਿਲ_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    ਜੇ (_PyTokenizer_IsInteractive(p->tok)) {
        ਵਾਪਸ 0;
    }

    PyObject *type, *ਮੁੱਲ, *traceback;
    PyErr_Fetch(&type, &ਮੁੱਲ, &traceback);

    Token *current_token = p->ਗਿਆਤ_err_token != ਨਿਰਾ ? p->ਗਿਆਤ_err_token : p->ਟੋਕਨ[p->fill - 1];
    Py_ssize_t current_err_ਸਤਰ = current_token->lineno;

    ਪੂਰਨ ret = 0;
    ਬਣਤਰ token new_token;
    _PyToken_Init(&new_token);

    ਲਈ (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        ਚੋਣ (new_token.type) {
            ਹਾਲਤ ERRORTOKEN: {
                ਜੇ (PyErr_Occurred()) {
                    ret = -1;
                    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                ਜੇ (info.level != 0) {
                    ਪੂਰਨ error_lineno = info.delimiter_loc.lineno;
                    ਜੇ (current_err_ਸਤਰ > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
                    }
                }
                ਰੋਕੋ;
            }
            ਹਾਲਤ ENDMARKER:
                ਰੋਕੋ;
            ਮੂਲ:
                ਜਾਰੀ;
        }
        ਰੋਕੋ;
    }


ਬਾਹਰ_ਨਿਕਲੋ:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    ਜੇ (PyErr_Occurred() && !info.ਵਿੱਚ_formatted_string) {
        Py_XDECREF(ਮੁੱਲ);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } ਨਹੀਂਤਾਂ {
        PyErr_Restore(type, ਮੁੱਲ, traceback);
    }
    ਵਾਪਸ ret;
}

// PARSER ERRORS

ਖਾਲੀ *
_PyPegen_raise_error(Parser *p, PyObject *errtype, ਪੂਰਨ use_mark, ਅਚਲ ਅੱਖਰ *errmsg, ...)
{
    // Bail out if we already have an error set.
    ਜੇ (p->error_indicator && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (p->fill == 0) {
        va_list va;
        va_ਸ਼ੁਰੂ(va, errmsg);
        Py_Pegen_raise_error_ਗਿਆਤ_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_ਸਿਰਾ(va);
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        ਵਾਪਸ ਨਿਰਾ;
    }
    Token *t = p->ਗਿਆਤ_err_token != ਨਿਰਾ
                   ? p->ਗਿਆਤ_err_token
                   : p->ਟੋਕਨ[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t ਖਾਨਾ_ਵਿੱਥ;
    Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = -1;
    ਜੇ (t->ਖਾਨਾ_ਵਿੱਥ == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        ਜੇ (info.diagnostic.location.lineno != 0) {
            ਖਾਨਾ_ਵਿੱਥ = info.diagnostic.location.byte_ਖਾਨਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (info.cursor == info.ਨਿਵੇਸ਼_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ) {
            ਖਾਨਾ_ਵਿੱਥ = 0;
        } ਨਹੀਂਤਾਂ {
            ਖਾਨਾ_ਵਿੱਥ = Py_ਸੁਰੱਖਿਅਤ_DOWNCAST(
                info.cursor - info.ਸਤਰ_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ, intptr_t, ਪੂਰਨ);
        }
    } ਨਹੀਂਤਾਂ {
        ਖਾਨਾ_ਵਿੱਥ = t->ਖਾਨਾ_ਵਿੱਥ + 1;
    }

    ਜੇ (t->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ != -1) {
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = t->ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ + 1;
    }

    va_list va;
    va_ਸ਼ੁਰੂ(va, errmsg);
    Py_Pegen_raise_error_ਗਿਆਤ_location(p, errtype, t->lineno, ਖਾਨਾ_ਵਿੱਥ, t->ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ, errmsg, va);
    va_ਸਿਰਾ(va);

    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ PyObject *
get_error_ਸਤਰ_from_ਸਰੋਤ(Parser *p, Py_ssize_t lineno)
{
    ਜੇ (Py_Tokenizer_Retained_ਸਰੋਤ(p->tok) == ਨਿਰਾ) {
        ਵਾਪਸ Py_GetConstant(Py_CONSTANT_ਖਾਲੀ_ਥਾਵਾਂ_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ਅਚਲ ਅੱਖਰ *ਸਤਰ = Py_Tokenizer_ਸਤਰ_View(p->tok, relative_lineno, &len);
    ਵਾਪਸ PyUnicode_DecodeUTF8(ਸਤਰ, len, "replace");
}

ਖਾਲੀ *
Py_Pegen_raise_error_ਗਿਆਤ_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t ਖਾਨਾ_ਵਿੱਥ,
                                    Py_ssize_t ਸਿਰਾ_lineno, Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ,
                                    ਅਚਲ ਅੱਖਰ *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    ਜੇ (p->error_indicator && PyErr_Occurred()) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    PyObject *ਮੁੱਲ = ਨਿਰਾ;
    PyObject *errstr = ਨਿਰਾ;
    PyObject *error_ਸਤਰ = ਨਿਰਾ;
    PyObject *tmp = ਨਿਰਾ;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_ਵਿਸਤਾਰ ਪਾਠ_ਵਿਸਤਾਰ = info.diagnostic.location.lineno != 0
        ? info.diagnostic.ਪਾਠ_ਵਿਸਤਾਰ : info.ਸਤਰ_ਵਿਸਤਾਰ;

    ਜੇ (ਸਿਰਾ_lineno == CURRENT_POS) {
        ਸਿਰਾ_lineno = location.lineno;
    }
    ਜੇ (ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ == CURRENT_POS) {
        ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ = location.byte_ਖਾਨਾ;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    ਜੇ (!errstr) {
        ਜਾਓ error;
    }

    ਜੇ (info.is_interactive && Py_Tokenizer_Retained_ਸਰੋਤ(p->tok) != ਨਿਰਾ) {
        error_ਸਤਰ = get_error_ਸਤਰ_from_ਸਰੋਤ(p, lineno);
    }
    ਨਹੀਂਤਾਂ ਜੇ (p->ਸ਼ੁਰੂ_ਨਿਯਮ == Py_file_ਨਿਵੇਸ਼) {
        error_ਸਤਰ = Py_Err_ਪ੍ਰੋਗਰਾਮ_Decoded_ਪਾਠ_Object(info.filename,
                                                     (ਪੂਰਨ) lineno, info.encoding);
    }

    ਜੇ (!error_ਸਤਰ) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        ਪ੍ਰਤਿਜ੍ਞਾ(!info.is_file || info.ਸਥਿਤੀ == E_EOF);

        ਜੇ (location.lineno <= lineno &&
                info.ਨਿਵੇਸ਼_ਵਿਸਤਾਰ.ਸਿਰਾ > info.ਨਿਵੇਸ਼_ਵਿਸਤਾਰ.ਸ਼ੁਰੂ) {
            Py_ssize_t ਮਾਪ;
            ਅਚਲ ਅੱਖਰ *ਸਤਰ = Py_Tokenizer_ਵਿਸਤਾਰ_View(
                p->tok, ਪਾਠ_ਵਿਸਤਾਰ, &ਮਾਪ);
            ਜੇ (errtype == Py_Exc_Incomplete_ਨਿਵੇਸ਼_Error && ਮਾਪ > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->ਸ਼ੁਰੂ_ਨਿਯਮ != Py_file_ਨਿਵੇਸ਼))) {
                ਮਾਪ--;
            }
            error_ਸਤਰ = PyUnicode_DecodeUTF8(ਸਤਰ, ਮਾਪ, "replace");
        }
        ਨਹੀਂਤਾਂ ਜੇ (!info.is_file) {
            error_ਸਤਰ = get_error_ਸਤਰ_from_ਸਰੋਤ(p, lineno);
        }
        ਨਹੀਂਤਾਂ {
            error_ਸਤਰ = Py_GetConstant(Py_CONSTANT_ਖਾਲੀ_ਥਾਵਾਂ_STR);
        }
        ਜੇ (!error_ਸਤਰ) {
            ਜਾਓ error;
        }
    }

    Py_ssize_t ਖਾਨਾ_ਸੰਖਿਆ = ਖਾਨਾ_ਵਿੱਥ;
    Py_ssize_t ਸਿਰਾ_ਖਾਨਾ_ਸੰਖਿਆ = ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ;

    ਖਾਨਾ_ਸੰਖਿਆ = Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ(error_ਸਤਰ, ਖਾਨਾ_ਵਿੱਥ);
    ਜੇ (ਖਾਨਾ_ਸੰਖਿਆ < 0) {
        ਜਾਓ error;
    }

    ਜੇ (ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ > 0) {
        ਸਿਰਾ_ਖਾਨਾ_ਸੰਖਿਆ = Py_Pegen_byte_ਵਿੱਥ_ਮੰਜ਼ਿਲ_character_ਵਿੱਥ(error_ਸਤਰ, ਸਿਰਾ_ਖਾਨਾ_ਵਿੱਥ);
        ਜੇ (ਸਿਰਾ_ਖਾਨਾ_ਸੰਖਿਆ < 0) {
            ਜਾਓ error;
        }
    }

    tmp = Py_Build_ਮੁੱਲ("(OnnNnn)", info.filename, lineno, ਖਾਨਾ_ਸੰਖਿਆ, error_ਸਤਰ, ਸਿਰਾ_lineno, ਸਿਰਾ_ਖਾਨਾ_ਸੰਖਿਆ);
    error_ਸਤਰ = ਨਿਰਾ;
    ਜੇ (!tmp) {
        ਜਾਓ error;
    }
    ਮੁੱਲ = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ਜੇ (!ਮੁੱਲ) {
        ਜਾਓ error;
    }
    Py_Err_ਸੈੱਟ_Object(errtype, ਮੁੱਲ);

    Py_DECREF(errstr);
    Py_DECREF(ਮੁੱਲ);
    ਵਾਪਸ ਨਿਰਾ;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_ਸਤਰ);
    ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ
Pypegen_ਸੈੱਟ_syntax_error(Parser* p, Token* ਆਖਰੀ_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    ਜੇ (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        ਪੂਰਨ is_tok_ok = (info.ਸਥਿਤੀ == E_ਪੂਰਾ || info.ਸਥਿਤੀ == E_OK);
        ਜੇ (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_ਭਰਿਆ_ਸਰੋਤ_ਮੰਜ਼ਿਲ_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        ਵਾਪਸ;
    }
    // Initialization error
    ਜੇ (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    ਜੇ (ਆਖਰੀ_token->type == ERRORTOKEN && info.ਸਥਿਤੀ == E_EOF) {
        ਜੇ (info.level) {
            raise_unclosed_parentheses_error(p);
        } ਨਹੀਂਤਾਂ {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        ਵਾਪਸ;
    }
    // Indentation error in the tokenizer
    ਜੇ (ਆਖਰੀ_token->type == INDENT || ਆਖਰੀ_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(ਆਖਰੀ_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        ਵਾਪਸ;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_ਗਿਆਤ_LOCATION(ਆਖਰੀ_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_ਭਰਿਆ_ਸਰੋਤ_ਮੰਜ਼ਿਲ_check_for_errors(p);
}

ਖਾਲੀ
Pypegen_ਢੇਰ_ਉਛਾਲ(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_ਸੈੱਟ_String(Py_Exc_ਸਮ੍ਰਿਤੀ_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
