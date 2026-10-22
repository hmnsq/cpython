#شامل <Python.h>
#شامل <errcode.h>

#شامل "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#شامل "pycore_runtime.h"       // _Py_ID()
#شامل "pycore_tuple.h"         // _PyTuple_FromPair
#شامل "pegen.h"
#شامل "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

تهی *
Py_Pegen_raise_incomplete_درون_داد(Parser *p)
{
    اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) && !PyErr_Occurred()) {
        بازگشت _PyPegen_raise_error(p, Py_Exc_Incomplete_درون_داد_Error, 0,
                                    "incomplete input");
    }
    بازگشت پوچ;
}

ایستا درونخطی تهی
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       صحیح error_lineno = info.delimiter_loc.lineno;
       صحیح error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

صحیح
_Pypegen_tokenizer_error(Parser *p)
{
    اگر (PyErr_Occurred()) {
        بازگشت -1;
    }

    ثابت نویسه *پیام = پوچ;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    اگر ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_درون_داد) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_درون_داد(p);
        بازگشت -1;
    }
    انتخاب (info.status) {
        حالت E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            بازگشت -1;
        حالت E_TOKEN:
            پیام = "invalid token";
            بشکن;
        حالت E_EOF:
            اگر (info.level) {
                raise_unclosed_parentheses_error(p);
            } وگرنه {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            بازگشت -1;
        حالت E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            بازگشت -1;
        حالت E_INTR:
            اگر (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            بازگشت -1;
        حالت E_NOMEM:
            PyErr_NoMemory();
            بازگشت -1;
        حالت E_TABSPACE:
            errtype = PyExc_TabError;
            پیام = "inconsistent use of tabs and spaces in indentation";
            بشکن;
        حالت E_TOODEEP:
            errtype = PyExc_IndentationError;
            پیام = "too many levels of indentation";
            بشکن;
        حالت E_LINECONT: {
            col_offset = info.cursor - info.سطر_span.start - 1;
            پیام = "unexpected character after line continuation character";
            بشکن;
        }
        حالت E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            بازگشت -1;
        پیشفرض:
            پیام = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, پیام);
    بازگشت -1;
}

صحیح
_Pypegen_raise_decode_error(Parser *p)
{
    أكد(PyErr_Occurred());
    ثابت نویسه *errtype = پوچ;
    اگر (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    وگرنه اگر (PyErr_ExceptionMatches(Py_Exc_مقدار_Error)) {
        errtype = "value error";
    }
    اگر (errtype) {
        PyObject *type;
        PyObject *مقدار;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &مقدار, &tback);
        errstr = PyObject_Str(مقدار);
        اگر (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        وگرنه {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(مقدار);
        Py_XDECREF(tback);
    }

    بازگشت -1;
}

ایستا صحیح
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    اگر (_PyTokenizer_IsInteractive(p->tok)) {
        بازگشت 0;
    }

    PyObject *type, *مقدار, *traceback;
    PyErr_Fetch(&type, &مقدار, &traceback);

    Token *current_token = p->known_err_token != پوچ ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_سطر = current_token->lineno;

    صحیح ret = 0;
    ساختار token new_token;
    _PyToken_Init(&new_token);

    برای (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        انتخاب (new_token.type) {
            حالت ERRORTOKEN: {
                اگر (PyErr_Occurred()) {
                    ret = -1;
                    برو exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                اگر (info.level != 0) {
                    صحیح error_lineno = info.delimiter_loc.lineno;
                    اگر (current_err_سطر > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        برو exit;
                    }
                }
                بشکن;
            }
            حالت ENDMARKER:
                بشکن;
            پیشفرض:
                ادامه;
        }
        بشکن;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    اگر (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(مقدار);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } وگرنه {
        PyErr_Restore(type, مقدار, traceback);
    }
    بازگشت ret;
}

// PARSER ERRORS

تهی *
_PyPegen_raise_error(Parser *p, PyObject *errtype, صحیح use_mark, ثابت نویسه *errmsg, ...)
{
    // Bail out if we already have an error set.
    اگر (p->error_indicator && PyErr_Occurred()) {
        بازگشت پوچ;
    }
    اگر (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        بازگشت پوچ;
    }
    اگر (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        بازگشت پوچ;
    }
    Token *t = p->known_err_token != پوچ
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    اگر (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        اگر (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } وگرنه اگر (info.cursor == info.درون_داد_span.start) {
            col_offset = 0;
        } وگرنه {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.سطر_span.start, intptr_t, صحیح);
        }
    } وگرنه {
        col_offset = t->col_offset + 1;
    }

    اگر (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    بازگشت پوچ;
}

ایستا PyObject *
get_error_سطر_from_source(Parser *p, Py_ssize_t lineno)
{
    اگر (_PyTokenizer_RetainedSource(p->tok) == پوچ) {
        بازگشت Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت نویسه *سطر = Py_Tokenizer_سطر_View(p->tok, relative_lineno, &len);
    بازگشت PyUnicode_DecodeUTF8(سطر, len, "replace");
}

تهی *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ثابت نویسه *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    اگر (p->error_indicator && PyErr_Occurred()) {
        بازگشت پوچ;
    }
    PyObject *مقدار = پوچ;
    PyObject *errstr = پوچ;
    PyObject *error_سطر = پوچ;
    PyObject *tmp = پوچ;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.سطر_span;

    اگر (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    اگر (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    اگر (!errstr) {
        برو error;
    }

    اگر (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != پوچ) {
        error_سطر = get_error_سطر_from_source(p, lineno);
    }
    وگرنه اگر (p->start_rule == Py_file_درون_داد) {
        error_سطر = _PyErr_ProgramDecodedTextObject(info.filename,
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
        أكد(!info.is_file || info.status == E_EOF);

        اگر (location.lineno <= lineno &&
                info.درون_داد_span.end > info.درون_داد_span.start) {
            Py_ssize_t size;
            ثابت نویسه *سطر = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            اگر (errtype == Py_Exc_Incomplete_درون_داد_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_درون_داد))) {
                size--;
            }
            error_سطر = PyUnicode_DecodeUTF8(سطر, size, "replace");
        }
        وگرنه اگر (!info.is_file) {
            error_سطر = get_error_سطر_from_source(p, lineno);
        }
        وگرنه {
            error_سطر = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        اگر (!error_سطر) {
            برو error;
        }
    }

    Py_ssize_t col_عدد = col_offset;
    Py_ssize_t end_col_عدد = end_col_offset;

    col_عدد = _PyPegen_byte_offset_to_character_offset(error_سطر, col_offset);
    اگر (col_عدد < 0) {
        برو error;
    }

    اگر (end_col_offset > 0) {
        end_col_عدد = _PyPegen_byte_offset_to_character_offset(error_سطر, end_col_offset);
        اگر (end_col_عدد < 0) {
            برو error;
        }
    }

    tmp = Py_Build_مقدار("(OnnNnn)", info.filename, lineno, col_عدد, error_سطر, end_lineno, end_col_عدد);
    error_سطر = پوچ;
    اگر (!tmp) {
        برو error;
    }
    مقدار = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    اگر (!مقدار) {
        برو error;
    }
    PyErr_SetObject(errtype, مقدار);

    Py_DECREF(errstr);
    Py_DECREF(مقدار);
    بازگشت پوچ;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_سطر);
    بازگشت پوچ;
}

تهی
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    اگر (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحیح is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        اگر (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        بازگشت;
    }
    // Initialization error
    اگر (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    اگر (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        اگر (info.level) {
            raise_unclosed_parentheses_error(p);
        } وگرنه {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        بازگشت;
    }
    // Indentation error in the tokenizer
    اگر (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        بازگشت;
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

تهی
Pypegen_پشته_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
