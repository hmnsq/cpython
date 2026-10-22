#ضمن <Python.h>
#ضمن <errcode.h>

#ضمن "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ضمن "pycore_runtime.h"       // _Py_ID()
#ضمن "pycore_tuple.h"         // _PyTuple_FromPair
#ضمن "pegen.h"
#ضمن "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

فراغ *
_PyPegen_raise_incomplete_input(Parser *p)
{
    אם ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) && !PyErr_Occurred()) {
        החזר _PyPegen_raise_error(p, PyExc_IncompleteInputError, 0,
                                    "incomplete input");
    }
    החזר عدم;
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
    אם (PyErr_Occurred()) {
        החזר -1;
    }

    ثابت حرف *msg = عدم;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    אם ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        _PyPegen_raise_incomplete_input(p);
        החזר -1;
    }
    بدل (info.status) {
        حالة E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            החזר -1;
        حالة E_TOKEN:
            msg = "invalid token";
            اكسر;
        حالة E_EOF:
            אם (info.level) {
                raise_unclosed_parentheses_error(p);
            } אחרת {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            החזר -1;
        حالة E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            החזר -1;
        حالة E_INTR:
            אם (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            החזר -1;
        حالة E_NOMEM:
            PyErr_NoMemory();
            החזר -1;
        حالة E_TABSPACE:
            errtype = PyExc_TabError;
            msg = "inconsistent use of tabs and spaces in indentation";
            اكسر;
        حالة E_TOODEEP:
            errtype = PyExc_IndentationError;
            msg = "too many levels of indentation";
            اكسر;
        حالة E_LINECONT: {
            col_offset = info.cursor - info.line_span.start - 1;
            msg = "unexpected character after line continuation character";
            اكسر;
        }
        حالة E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            החזר -1;
        افتراضي:
            msg = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, msg);
    החזר -1;
}

صحيح
_Pypegen_raise_decode_error(Parser *p)
{
    أكد(PyErr_Occurred());
    ثابت حرف *errtype = عدم;
    אם (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    אחרת אם (PyErr_ExceptionMatches(PyExc_ValueError)) {
        errtype = "value error";
    }
    אם (errtype) {
        PyObject *type;
        PyObject *value;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &value, &tback);
        errstr = PyObject_Str(value);
        אם (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        אחרת {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(tback);
    }

    החזר -1;
}

ساكن صحيح
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    אם (_PyTokenizer_IsInteractive(p->tok)) {
        החזר 0;
    }

    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);

    Token *current_token = p->known_err_token != عدم ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_line = current_token->lineno;

    صحيح ret = 0;
    بنية token new_token;
    _PyToken_Init(&new_token);

    עבור (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        بدل (new_token.type) {
            حالة ERRORTOKEN: {
                אם (PyErr_Occurred()) {
                    ret = -1;
                    اذهب exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                אם (info.level != 0) {
                    صحيح error_lineno = info.delimiter_loc.lineno;
                    אם (current_err_line > error_lineno) {
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
    אם (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(value);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } אחרת {
        PyErr_Restore(type, value, traceback);
    }
    החזר ret;
}

// PARSER ERRORS

فراغ *
_PyPegen_raise_error(Parser *p, PyObject *errtype, صحيح use_mark, ثابت حرف *errmsg, ...)
{
    // Bail out if we already have an error set.
    אם (p->error_indicator && PyErr_Occurred()) {
        החזר عدم;
    }
    אם (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        החזר عدم;
    }
    אם (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        החזר عدم;
    }
    Token *t = p->known_err_token != عدم
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    אם (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        אם (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } אחרת אם (info.cursor == info.input_span.start) {
            col_offset = 0;
        } אחרת {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.line_span.start, intptr_t, صحيح);
        }
    } אחרת {
        col_offset = t->col_offset + 1;
    }

    אם (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    החזר عدم;
}

ساكن PyObject *
get_error_line_from_source(Parser *p, Py_ssize_t lineno)
{
    אם (_PyTokenizer_RetainedSource(p->tok) == عدم) {
        החזר Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ثابت حرف *line = _PyTokenizer_LineView(p->tok, relative_lineno, &len);
    החזר PyUnicode_DecodeUTF8(line, len, "replace");
}

فراغ *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ثابت حرف *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    אם (p->error_indicator && PyErr_Occurred()) {
        החזר عدم;
    }
    PyObject *value = عدم;
    PyObject *errstr = عدم;
    PyObject *error_line = عدم;
    PyObject *tmp = عدم;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.line_span;

    אם (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    אם (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    אם (!errstr) {
        اذهب error;
    }

    אם (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != عدم) {
        error_line = get_error_line_from_source(p, lineno);
    }
    אחרת אם (p->start_rule == Py_file_input) {
        error_line = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (صحيح) lineno, info.encoding);
    }

    אם (!error_line) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        أكد(!info.is_file || info.status == E_EOF);

        אם (location.lineno <= lineno &&
                info.input_span.end > info.input_span.start) {
            Py_ssize_t size;
            ثابت حرف *line = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            אם (errtype == PyExc_IncompleteInputError && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_input))) {
                size--;
            }
            error_line = PyUnicode_DecodeUTF8(line, size, "replace");
        }
        אחרת אם (!info.is_file) {
            error_line = get_error_line_from_source(p, lineno);
        }
        אחרת {
            error_line = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        אם (!error_line) {
            اذهب error;
        }
    }

    Py_ssize_t col_number = col_offset;
    Py_ssize_t end_col_number = end_col_offset;

    col_number = _PyPegen_byte_offset_to_character_offset(error_line, col_offset);
    אם (col_number < 0) {
        اذهب error;
    }

    אם (end_col_offset > 0) {
        end_col_number = _PyPegen_byte_offset_to_character_offset(error_line, end_col_offset);
        אם (end_col_number < 0) {
            اذهب error;
        }
    }

    tmp = Py_BuildValue("(OnnNnn)", info.filename, lineno, col_number, error_line, end_lineno, end_col_number);
    error_line = عدم;
    אם (!tmp) {
        اذهب error;
    }
    value = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    אם (!value) {
        اذهب error;
    }
    PyErr_SetObject(errtype, value);

    Py_DECREF(errstr);
    Py_DECREF(value);
    החזר عدم;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_line);
    החזר عدم;
}

فراغ
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    אם (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        صحيح is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        אם (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        החזר;
    }
    // Initialization error
    אם (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    אם (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        אם (info.level) {
            raise_unclosed_parentheses_error(p);
        } אחרת {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        החזר;
    }
    // Indentation error in the tokenizer
    אם (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        החזר;
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
_Pypegen_stack_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
