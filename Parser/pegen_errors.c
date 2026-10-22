#include <Python.h>
#include <errcode.h>

#include "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#include "pycore_runtime.h"       // _Py_ID()
#include "pycore_tuple.h"         // _PyTuple_FromPair
#include "pegen.h"
#include "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

void *
_PyPegen_raise_incomplete_input(Parser *p)
{
    ပေႃး ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) && !PyErr_Occurred()) {
        ပွၵ်ႈ _PyPegen_raise_error(p, PyExc_IncompleteInputError, 0,
                                    "incomplete input");
    }
    ပွၵ်ႈ NULL;
}

static anta_hstha void
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       puur_naa_nka error_lineno = info.delimiter_loc.lineno;
       puur_naa_nka error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

puur_naa_nka
_Pypegen_tokenizer_error(Parser *p)
{
    ပေႃး (PyErr_Occurred()) {
        ပွၵ်ႈ -1;
    }

    const var_na *msg = NULL;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ပေႃး ((p->flags & PyPARSE_ALLOW_INCOMPLETE_INPUT) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        _PyPegen_raise_incomplete_input(p);
        ပွၵ်ႈ -1;
    }
    switch (info.status) {
        pak_sa E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            ပွၵ်ႈ -1;
        pak_sa E_TOKEN:
            msg = "invalid token";
            bha_nga;
        pak_sa E_EOF:
            ပေႃး (info.level) {
                raise_unclosed_parentheses_error(p);
            } ပေႃးဢမ်ႇ {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            ပွၵ်ႈ -1;
        pak_sa E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            ပွၵ်ႈ -1;
        pak_sa E_INTR:
            ပေႃး (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            ပွၵ်ႈ -1;
        pak_sa E_NOMEM:
            PyErr_NoMemory();
            ပွၵ်ႈ -1;
        pak_sa E_TABSPACE:
            errtype = PyExc_TabError;
            msg = "inconsistent use of tabs and spaces in indentation";
            bha_nga;
        pak_sa E_TOODEEP:
            errtype = PyExc_IndentationError;
            msg = "too many levels of indentation";
            bha_nga;
        pak_sa E_LINECONT: {
            col_offset = info.cursor - info.line_span.start - 1;
            msg = "unexpected character after line continuation character";
            bha_nga;
        }
        pak_sa E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            ပွၵ်ႈ -1;
        default:
            msg = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, msg);
    ပွၵ်ႈ -1;
}

puur_naa_nka
_Pypegen_raise_decode_error(Parser *p)
{
    assert(PyErr_Occurred());
    const var_na *errtype = NULL;
    ပေႃး (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    ပေႃးဢမ်ႇ ပေႃး (PyErr_ExceptionMatches(PyExc_ValueError)) {
        errtype = "value error";
    }
    ပေႃး (errtype) {
        PyObject *type;
        PyObject *value;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &value, &tback);
        errstr = PyObject_Str(value);
        ပေႃး (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        ပေႃးဢမ်ႇ {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(tback);
    }

    ပွၵ်ႈ -1;
}

static puur_naa_nka
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    ပေႃး (_PyTokenizer_IsInteractive(p->tok)) {
        ပွၵ်ႈ 0;
    }

    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);

    Token *current_token = p->known_err_token != NULL ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_line = current_token->lineno;

    puur_naa_nka ret = 0;
    sa_nracanaa token new_token;
    _PyToken_Init(&new_token);

    for (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        switch (new_token.type) {
            pak_sa ERRORTOKEN: {
                ပေႃး (PyErr_Occurred()) {
                    ret = -1;
                    goto exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                ပေႃး (info.level != 0) {
                    puur_naa_nka error_lineno = info.delimiter_loc.lineno;
                    ပေႃး (current_err_line > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        goto exit;
                    }
                }
                bha_nga;
            }
            pak_sa ENDMARKER:
                bha_nga;
            default:
                continue;
        }
        bha_nga;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    ပေႃး (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(value);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } ပေႃးဢမ်ႇ {
        PyErr_Restore(type, value, traceback);
    }
    ပွၵ်ႈ ret;
}

// PARSER ERRORS

void *
_PyPegen_raise_error(Parser *p, PyObject *errtype, puur_naa_nka use_mark, const var_na *errmsg, ...)
{
    // Bail out if we already have an error set.
    ပေႃး (p->error_indicator && PyErr_Occurred()) {
        ပွၵ်ႈ NULL;
    }
    ပေႃး (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        ပွၵ်ႈ NULL;
    }
    ပေႃး (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        ပွၵ်ႈ NULL;
    }
    Token *t = p->known_err_token != NULL
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    ပေႃး (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        ပေႃး (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } ပေႃးဢမ်ႇ ပေႃး (info.cursor == info.input_span.start) {
            col_offset = 0;
        } ပေႃးဢမ်ႇ {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.line_span.start, intptr_t, puur_naa_nka);
        }
    } ပေႃးဢမ်ႇ {
        col_offset = t->col_offset + 1;
    }

    ပေႃး (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    ပွၵ်ႈ NULL;
}

static PyObject *
get_error_line_from_source(Parser *p, Py_ssize_t lineno)
{
    ပေႃး (_PyTokenizer_RetainedSource(p->tok) == NULL) {
        ပွၵ်ႈ Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    const var_na *line = _PyTokenizer_LineView(p->tok, relative_lineno, &len);
    ပွၵ်ႈ PyUnicode_DecodeUTF8(line, len, "replace");
}

void *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    const var_na *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    ပေႃး (p->error_indicator && PyErr_Occurred()) {
        ပွၵ်ႈ NULL;
    }
    PyObject *value = NULL;
    PyObject *errstr = NULL;
    PyObject *error_line = NULL;
    PyObject *tmp = NULL;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.line_span;

    ပေႃး (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    ပေႃး (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    ပေႃး (!errstr) {
        goto error;
    }

    ပေႃး (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != NULL) {
        error_line = get_error_line_from_source(p, lineno);
    }
    ပေႃးဢမ်ႇ ပေႃး (p->start_rule == Py_file_input) {
        error_line = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (puur_naa_nka) lineno, info.encoding);
    }

    ပေႃး (!error_line) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        assert(!info.is_file || info.status == E_EOF);

        ပေႃး (location.lineno <= lineno &&
                info.input_span.end > info.input_span.start) {
            Py_ssize_t size;
            const var_na *line = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            ပေႃး (errtype == PyExc_IncompleteInputError && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_input))) {
                size--;
            }
            error_line = PyUnicode_DecodeUTF8(line, size, "replace");
        }
        ပေႃးဢမ်ႇ ပေႃး (!info.is_file) {
            error_line = get_error_line_from_source(p, lineno);
        }
        ပေႃးဢမ်ႇ {
            error_line = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        ပေႃး (!error_line) {
            goto error;
        }
    }

    Py_ssize_t col_number = col_offset;
    Py_ssize_t end_col_number = end_col_offset;

    col_number = _PyPegen_byte_offset_to_character_offset(error_line, col_offset);
    ပေႃး (col_number < 0) {
        goto error;
    }

    ပေႃး (end_col_offset > 0) {
        end_col_number = _PyPegen_byte_offset_to_character_offset(error_line, end_col_offset);
        ပေႃး (end_col_number < 0) {
            goto error;
        }
    }

    tmp = Py_BuildValue("(OnnNnn)", info.filename, lineno, col_number, error_line, end_lineno, end_col_number);
    error_line = NULL;
    ပေႃး (!tmp) {
        goto error;
    }
    value = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ပေႃး (!value) {
        goto error;
    }
    PyErr_SetObject(errtype, value);

    Py_DECREF(errstr);
    Py_DECREF(value);
    ပွၵ်ႈ NULL;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_line);
    ပွၵ်ႈ NULL;
}

void
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    ပေႃး (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        puur_naa_nka is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        ပေႃး (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        ပွၵ်ႈ;
    }
    // Initialization error
    ပေႃး (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    ပေႃး (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        ပေႃး (info.level) {
            raise_unclosed_parentheses_error(p);
        } ပေႃးဢမ်ႇ {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        ပွၵ်ႈ;
    }
    // Indentation error in the tokenizer
    ပေႃး (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        ပွၵ်ႈ;
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

void
_Pypegen_stack_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
