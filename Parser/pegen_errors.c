#અન્તર્ભાવ <Python.h>
#અન્તર્ભાવ <errcode.h>

#અન્તર્ભાવ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#અન્તર્ભાવ "pycore_runtime.h"       // _Py_ID()
#અન્તર્ભાવ "pycore_tuple.h"         // _PyTuple_FromPair
#અન્તર્ભાવ "pegen.h"
#અન્તર્ભાવ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

શૂન્ય *
Py_Pegen_raise_incomplete_આગત(Parser *p)
{
    જો ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_આગત) && !PyErr_Occurred()) {
        પાછા _PyPegen_raise_error(p, Py_Exc_Incomplete_આગત_Error, 0,
                                    "incomplete input");
    }
    પાછા ખાલી;
}

સ્થિર અંતઃરેખ શૂન્ય
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       પૂર્ણાંક error_lineno = info.delimiter_loc.lineno;
       પૂર્ણાંક error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

પૂર્ણાંક
_Pypegen_tokenizer_error(Parser *p)
{
    જો (PyErr_Occurred()) {
        પાછા -1;
    }

    અચલ અક્ષર *સંદેશ = ખાલી;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    જો ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_આગત) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_આગત(p);
        પાછા -1;
    }
    પસંદગી (info.status) {
        સ્થિતિ E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            પાછા -1;
        સ્થિતિ E_TOKEN:
            સંદેશ = "invalid token";
            તોડો;
        સ્થિતિ E_EOF:
            જો (info.level) {
                raise_unclosed_parentheses_error(p);
            } નહીંતર {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            પાછા -1;
        સ્થિતિ E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            પાછા -1;
        સ્થિતિ E_INTR:
            જો (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            પાછા -1;
        સ્થિતિ E_NOMEM:
            PyErr_NoMemory();
            પાછા -1;
        સ્થિતિ E_TABSPACE:
            errtype = PyExc_TabError;
            સંદેશ = "inconsistent use of tabs and spaces in indentation";
            તોડો;
        સ્થિતિ E_TOODEEP:
            errtype = PyExc_IndentationError;
            સંદેશ = "too many levels of indentation";
            તોડો;
        સ્થિતિ E_LINECONT: {
            col_offset = info.cursor - info.લીટી_span.start - 1;
            સંદેશ = "unexpected character after line continuation character";
            તોડો;
        }
        સ્થિતિ E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            પાછા -1;
        મૂળભૂત:
            સંદેશ = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, સંદેશ);
    પાછા -1;
}

પૂર્ણાંક
_Pypegen_raise_decode_error(Parser *p)
{
    પ્રતિજ્ઞા(PyErr_Occurred());
    અચલ અક્ષર *errtype = ખાલી;
    જો (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    નહીંતર જો (PyErr_ExceptionMatches(Py_Exc_કિંમત_Error)) {
        errtype = "value error";
    }
    જો (errtype) {
        PyObject *type;
        PyObject *કિંમત;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &કિંમત, &tback);
        errstr = PyObject_Str(કિંમત);
        જો (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        નહીંતર {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(કિંમત);
        Py_XDECREF(tback);
    }

    પાછા -1;
}

સ્થિર પૂર્ણાંક
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    જો (_PyTokenizer_IsInteractive(p->tok)) {
        પાછા 0;
    }

    PyObject *type, *કિંમત, *traceback;
    PyErr_Fetch(&type, &કિંમત, &traceback);

    Token *current_token = p->known_err_token != ખાલી ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_લીટી = current_token->lineno;

    પૂર્ણાંક ret = 0;
    સંરચના token new_token;
    _PyToken_Init(&new_token);

    માટે (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        પસંદગી (new_token.type) {
            સ્થિતિ ERRORTOKEN: {
                જો (PyErr_Occurred()) {
                    ret = -1;
                    જાઓ exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                જો (info.level != 0) {
                    પૂર્ણાંક error_lineno = info.delimiter_loc.lineno;
                    જો (current_err_લીટી > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        જાઓ exit;
                    }
                }
                તોડો;
            }
            સ્થિતિ ENDMARKER:
                તોડો;
            મૂળભૂત:
                ચાલુ;
        }
        તોડો;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    જો (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(કિંમત);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } નહીંતર {
        PyErr_Restore(type, કિંમત, traceback);
    }
    પાછા ret;
}

// PARSER ERRORS

શૂન્ય *
_PyPegen_raise_error(Parser *p, PyObject *errtype, પૂર્ણાંક use_mark, અચલ અક્ષર *errmsg, ...)
{
    // Bail out if we already have an error set.
    જો (p->error_indicator && PyErr_Occurred()) {
        પાછા ખાલી;
    }
    જો (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        પાછા ખાલી;
    }
    જો (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        પાછા ખાલી;
    }
    Token *t = p->known_err_token != ખાલી
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    જો (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        જો (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } નહીંતર જો (info.cursor == info.આગત_span.start) {
            col_offset = 0;
        } નહીંતર {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.લીટી_span.start, intptr_t, પૂર્ણાંક);
        }
    } નહીંતર {
        col_offset = t->col_offset + 1;
    }

    જો (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    પાછા ખાલી;
}

સ્થિર PyObject *
get_error_લીટી_from_source(Parser *p, Py_ssize_t lineno)
{
    જો (_PyTokenizer_RetainedSource(p->tok) == ખાલી) {
        પાછા Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    અચલ અક્ષર *લીટી = Py_Tokenizer_લીટી_View(p->tok, relative_lineno, &len);
    પાછા PyUnicode_DecodeUTF8(લીટી, len, "replace");
}

શૂન્ય *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    અચલ અક્ષર *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    જો (p->error_indicator && PyErr_Occurred()) {
        પાછા ખાલી;
    }
    PyObject *કિંમત = ખાલી;
    PyObject *errstr = ખાલી;
    PyObject *error_લીટી = ખાલી;
    PyObject *tmp = ખાલી;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.લીટી_span;

    જો (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    જો (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    જો (!errstr) {
        જાઓ error;
    }

    જો (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != ખાલી) {
        error_લીટી = get_error_લીટી_from_source(p, lineno);
    }
    નહીંતર જો (p->start_rule == Py_file_આગત) {
        error_લીટી = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (પૂર્ણાંક) lineno, info.encoding);
    }

    જો (!error_લીટી) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        પ્રતિજ્ઞા(!info.is_file || info.status == E_EOF);

        જો (location.lineno <= lineno &&
                info.આગત_span.end > info.આગત_span.start) {
            Py_ssize_t size;
            અચલ અક્ષર *લીટી = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            જો (errtype == Py_Exc_Incomplete_આગત_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_આગત))) {
                size--;
            }
            error_લીટી = PyUnicode_DecodeUTF8(લીટી, size, "replace");
        }
        નહીંતર જો (!info.is_file) {
            error_લીટી = get_error_લીટી_from_source(p, lineno);
        }
        નહીંતર {
            error_લીટી = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        જો (!error_લીટી) {
            જાઓ error;
        }
    }

    Py_ssize_t col_સંખ્યા = col_offset;
    Py_ssize_t end_col_સંખ્યા = end_col_offset;

    col_સંખ્યા = _PyPegen_byte_offset_to_character_offset(error_લીટી, col_offset);
    જો (col_સંખ્યા < 0) {
        જાઓ error;
    }

    જો (end_col_offset > 0) {
        end_col_સંખ્યા = _PyPegen_byte_offset_to_character_offset(error_લીટી, end_col_offset);
        જો (end_col_સંખ્યા < 0) {
            જાઓ error;
        }
    }

    tmp = Py_Build_કિંમત("(OnnNnn)", info.filename, lineno, col_સંખ્યા, error_લીટી, end_lineno, end_col_સંખ્યા);
    error_લીટી = ખાલી;
    જો (!tmp) {
        જાઓ error;
    }
    કિંમત = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    જો (!કિંમત) {
        જાઓ error;
    }
    PyErr_SetObject(errtype, કિંમત);

    Py_DECREF(errstr);
    Py_DECREF(કિંમત);
    પાછા ખાલી;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_લીટી);
    પાછા ખાલી;
}

શૂન્ય
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    જો (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        પૂર્ણાંક is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        જો (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        પાછા;
    }
    // Initialization error
    જો (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    જો (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        જો (info.level) {
            raise_unclosed_parentheses_error(p);
        } નહીંતર {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        પાછા;
    }
    // Indentation error in the tokenizer
    જો (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        પાછા;
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

શૂન્ય
Pypegen_ઢગલો_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
