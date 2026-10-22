#ضمن <Python.h>
#ضمن <errcode.h>

#ضمن "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair
#ضمن "pegen.h"
#ضمن "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

فراغ *
Py_Pegen_raise_incomplete_كىرگۈزۈش(Parser *p)
{
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_كىرگۈزۈش) && !PyErr_Occurred()) {
        أرجع _PyPegen_raise_error(p, Py_Exc_Incomplete_كىرگۈزۈش_Error, 0,
                                    "incomplete input");
    }
    أرجع عدم;
}

ساكن مضمن فراغ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       صحيح error_lineno = info.delimiter_loc.lineno;
       صحيح error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

صحيح
_Pypegen_tokenizer_error(Parser *p)
{
    إذا (PyErr_Occurred()) {
        أرجع -1;
    }

    ثابت حرف *ئۇچۇر = عدم;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    إذا ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_كىرگۈزۈش) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_كىرگۈزۈش(p);
        أرجع -1;
    }
    بدل (info.status) {
        حالة E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            أرجع -1;
        حالة E_TOKEN:
            ئۇچۇر = "invalid token";
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
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            أرجع -1;
        حالة E_NOMEM:
            PyErr_NoMemory();
            أرجع -1;
        حالة E_TABSPACE:
            errtype = PyExc_TabError;
            ئۇچۇر = "inconsistent use of tabs and spaces in indentation";
            اكسر;
        حالة E_TOODEEP:
            errtype = PyExc_IndentationError;
            ئۇچۇر = "too many levels of indentation";
            اكسر;
        حالة E_LINECONT: {
            col_offset = info.cursor - info.قۇر_span.start - 1;
            ئۇچۇر = "unexpected character after line continuation character";
            اكسر;
        }
        حالة E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            أرجع -1;
        افتراضي:
            ئۇچۇر = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, ئۇچۇر);
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
    وإلا إذا (PyErr_ExceptionMatches(Py_Exc_قىممەت_Error)) {
        errtype = "value error";
    }
    إذا (errtype) {
        PyObject *type;
        PyObject *قىممەت;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &قىممەت, &tback);
        errstr = PyObject_Str(قىممەت);
        إذا (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        وإلا {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(قىممەت);
        Py_XDECREF(tback);
    }

    أرجع -1;
}

ساكن صحيح
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    إذا (_PyTokenizer_IsInteractive(p->tok)) {
        أرجع 0;
    }

    PyObject *type, *قىممەت, *traceback;
    PyErr_Fetch(&type, &قىممەت, &traceback);

    Token *current_token = p->known_err_token != عدم ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_قۇر = current_token->lineno;

    صحيح ret = 0;
    بنية token new_token;
    _PyToken_Init(&new_token);

    لكل (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        بدل (new_token.type) {
            حالة ERRORTOKEN: {
                إذا (PyErr_Occurred()) {
                    ret = -1;
                    اذهب exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                إذا (info.level != 0) {
                    صحيح error_lineno = info.delimiter_loc.lineno;
                    إذا (current_err_قۇر > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        اذهب exit;
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


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    إذا (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(قىممەت);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } وإلا {
        PyErr_Restore(type, قىممەت, traceback);
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
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        أرجع عدم;
    }
    إذا (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        أرجع عدم;
    }
    Token *t = p->known_err_token != عدم
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    إذا (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        إذا (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } وإلا إذا (info.cursor == info.كىرگۈزۈش_span.start) {
            col_offset = 0;
        } وإلا {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.قۇر_span.start, intptr_t, صحيح);
        }
    } وإلا {
        col_offset = t->col_offset + 1;
    }

    إذا (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    أرجع عدم;
}

ساكن PyObject *
get_error_قۇر_from_source(Parser *p, Py_ssize_t lineno)
{
    إذا (_PyTokenizer_RetainedSource(p->tok) == عدم) {
        أرجع Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *قۇر = Py_Tokenizer_قۇر_View(p->tok, relative_lineno, &len);
    أرجع PyUnicode_DecodeUTF8(قۇر, len, "replace");
}

فراغ *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    إذا (p->error_indicator && PyErr_Occurred()) {
        أرجع عدم;
    }
    PyObject *قىممەت = عدم;
    PyObject *errstr = عدم;
    PyObject *error_قۇر = عدم;
    PyObject *tmp = عدم;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.قۇر_span;

    إذا (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    إذا (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    إذا (!errstr) {
        اذهب error;
    }

    إذا (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != عدم) {
        error_قۇر = get_error_قۇر_from_source(p, lineno);
    }
    وإلا إذا (p->start_rule == Py_file_كىرگۈزۈش) {
        error_قۇر = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (صحيح) lineno, info.encoding);
    }

    إذا (!error_قۇر) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.status == E_EOF);

        إذا (location.lineno <= lineno &&
                info.كىرگۈزۈش_span.end > info.كىرگۈزۈش_span.start) {
            Py_ssize_t size;
            ثابت حرف *قۇر = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            إذا (errtype == Py_Exc_Incomplete_كىرگۈزۈش_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_كىرگۈزۈش))) {
                size--;
            }
            error_قۇر = PyUnicode_DecodeUTF8(قۇر, size, "replace");
        }
        وإلا إذا (!info.is_file) {
            error_قۇر = get_error_قۇر_from_source(p, lineno);
        }
        وإلا {
            error_قۇر = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        إذا (!error_قۇر) {
            اذهب error;
        }
    }

    Py_ssize_t col_سان = col_offset;
    Py_ssize_t end_col_سان = end_col_offset;

    col_سان = _PyPegen_byte_offset_to_character_offset(error_قۇر, col_offset);
    إذا (col_سان < 0) {
        اذهب error;
    }

    إذا (end_col_offset > 0) {
        end_col_سان = _PyPegen_byte_offset_to_character_offset(error_قۇر, end_col_offset);
        إذا (end_col_سان < 0) {
            اذهب error;
        }
    }

    tmp = Py_Build_قىممەت("(OnnNnn)", info.filename, lineno, col_سان, error_قۇر, end_lineno, end_col_سان);
    error_قۇر = عدم;
    إذا (!tmp) {
        اذهب error;
    }
    قىممەت = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    إذا (!قىممەت) {
        اذهب error;
    }
    PyErr_SetObject(errtype, قىممەت);

    Py_DECREF(errstr);
    Py_DECREF(قىممەت);
    أرجع عدم;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_قۇر);
    أرجع عدم;
}

فراغ
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    إذا (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحيح is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        إذا (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        أرجع;
    }
    // Initialization error
    إذا (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    إذا (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        إذا (info.level) {
            raise_unclosed_parentheses_error(p);
        } وإلا {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        أرجع;
    }
    // Indentation error in the tokenizer
    إذا (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        أرجع;
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

فراغ
Pypegen_دۆۋە_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
