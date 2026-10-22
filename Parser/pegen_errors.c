#ଅନ୍ତର୍ଭାଵ <Python.h>
#ଅନ୍ତର୍ଭାଵ <errcode.h>

#ଅନ୍ତର୍ଭାଵ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ଅନ୍ତର୍ଭାଵ "pycore_runtime.h"       // _Py_ID()
#ଅନ୍ତର୍ଭାଵ "pycore_tuple.h"         // _PyTuple_FromPair
#ଅନ୍ତର୍ଭାଵ "pegen.h"
#ଅନ୍ତର୍ଭାଵ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

ଶୂନ୍ୟ *
Py_Pegen_raise_incomplete_ନିବେଶ(Parser *p)
{
    ଯଦି ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ) && !PyErr_Occurred()) {
        ଫେରନ୍ତୁ _PyPegen_raise_error(p, Py_Exc_Incomplete_ନିବେଶ_Error, 0,
                                    "incomplete input");
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_lineno = info.delimiter_loc.lineno;
       ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_Pypegen_tokenizer_error(Parser *p)
{
    ଯଦି (PyErr_Occurred()) {
        ଫେରନ୍ତୁ -1;
    }

    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = ଖାଲି;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ଯଦି ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ନିବେଶ) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_ନିବେଶ(p);
        ଫେରନ୍ତୁ -1;
    }
    ବାଛ (info.status) {
        ସ୍ଥିତି E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            ଫେରନ୍ତୁ -1;
        ସ୍ଥିତି E_TOKEN:
            ସନ୍ଦେଶ = "invalid token";
            ଭାଙ୍ଗ;
        ସ୍ଥିତି E_EOF:
            ଯଦି (info.level) {
                raise_unclosed_parentheses_error(p);
            } ଅନ୍ୟଥା {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            ଫେରନ୍ତୁ -1;
        ସ୍ଥିତି E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            ଫେରନ୍ତୁ -1;
        ସ୍ଥିତି E_INTR:
            ଯଦି (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            ଫେରନ୍ତୁ -1;
        ସ୍ଥିତି E_NOMEM:
            PyErr_NoMemory();
            ଫେରନ୍ତୁ -1;
        ସ୍ଥିତି E_TABSPACE:
            errtype = PyExc_TabError;
            ସନ୍ଦେଶ = "inconsistent use of tabs and spaces in indentation";
            ଭାଙ୍ଗ;
        ସ୍ଥିତି E_TOODEEP:
            errtype = PyExc_IndentationError;
            ସନ୍ଦେଶ = "too many levels of indentation";
            ଭାଙ୍ଗ;
        ସ୍ଥିତି E_LINECONT: {
            col_offset = info.cursor - info.ଧାଡ଼ି_span.start - 1;
            ସନ୍ଦେଶ = "unexpected character after line continuation character";
            ଭାଙ୍ଗ;
        }
        ସ୍ଥିତି E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            ଫେରନ୍ତୁ -1;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ସନ୍ଦେଶ = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_Pypegen_raise_decode_error(Parser *p)
{
    ପ୍ରତିଜ୍ଞା(PyErr_Occurred());
    ଅଚଳ ଅକ୍ଷର *errtype = ଖାଲି;
    ଯଦି (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    ଅନ୍ୟଥା ଯଦି (PyErr_ExceptionMatches(Py_Exc_ମୂଲ୍ୟ_Error)) {
        errtype = "value error";
    }
    ଯଦି (errtype) {
        PyObject *type;
        PyObject *ମୂଲ୍ୟ;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &ମୂଲ୍ୟ, &tback);
        errstr = PyObject_Str(ମୂଲ୍ୟ);
        ଯଦି (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        ଅନ୍ୟଥା {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(ମୂଲ୍ୟ);
        Py_XDECREF(tback);
    }

    ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    ଯଦି (_PyTokenizer_IsInteractive(p->tok)) {
        ଫେରନ୍ତୁ 0;
    }

    PyObject *type, *ମୂଲ୍ୟ, *traceback;
    PyErr_Fetch(&type, &ମୂଲ୍ୟ, &traceback);

    Token *current_token = p->known_err_token != ଖାଲି ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_ଧାଡ଼ି = current_token->lineno;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
    ସଂରଚନା token new_token;
    _PyToken_Init(&new_token);

    ପାଇଁ (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        ବାଛ (new_token.type) {
            ସ୍ଥିତି ERRORTOKEN: {
                ଯଦି (PyErr_Occurred()) {
                    ret = -1;
                    ଯାଅ exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                ଯଦି (info.level != 0) {
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_lineno = info.delimiter_loc.lineno;
                    ଯଦି (current_err_ଧାଡ଼ି > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        ଯାଅ exit;
                    }
                }
                ଭାଙ୍ଗ;
            }
            ସ୍ଥିତି ENDMARKER:
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଜାରି;
        }
        ଭାଙ୍ଗ;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    ଯଦି (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(ମୂଲ୍ୟ);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } ଅନ୍ୟଥା {
        PyErr_Restore(type, ମୂଲ୍ୟ, traceback);
    }
    ଫେରନ୍ତୁ ret;
}

// PARSER ERRORS

ଶୂନ୍ୟ *
_PyPegen_raise_error(Parser *p, PyObject *errtype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_mark, ଅଚଳ ଅକ୍ଷର *errmsg, ...)
{
    // Bail out if we already have an error set.
    ଯଦି (p->error_indicator && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        ଫେରନ୍ତୁ ଖାଲି;
    }
    Token *t = p->known_err_token != ଖାଲି
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    ଯଦି (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        ଯଦି (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } ଅନ୍ୟଥା ଯଦି (info.cursor == info.ନିବେଶ_span.start) {
            col_offset = 0;
        } ଅନ୍ୟଥା {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.ଧାଡ଼ି_span.start, intptr_t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        }
    } ଅନ୍ୟଥା {
        col_offset = t->col_offset + 1;
    }

    ଯଦି (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର PyObject *
get_error_ଧାଡ଼ି_from_source(Parser *p, Py_ssize_t lineno)
{
    ଯଦି (_PyTokenizer_RetainedSource(p->tok) == ଖାଲି) {
        ଫେରନ୍ତୁ Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = Py_Tokenizer_ଧାଡ଼ି_View(p->tok, relative_lineno, &len);
    ଫେରନ୍ତୁ PyUnicode_DecodeUTF8(ଧାଡ଼ି, len, "replace");
}

ଶୂନ୍ୟ *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    ଅଚଳ ଅକ୍ଷର *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    ଯଦି (p->error_indicator && PyErr_Occurred()) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    PyObject *ମୂଲ୍ୟ = ଖାଲି;
    PyObject *errstr = ଖାଲି;
    PyObject *error_ଧାଡ଼ି = ଖାଲି;
    PyObject *tmp = ଖାଲି;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.ଧାଡ଼ି_span;

    ଯଦି (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    ଯଦି (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    ଯଦି (!errstr) {
        ଯାଅ error;
    }

    ଯଦି (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != ଖାଲି) {
        error_ଧାଡ଼ି = get_error_ଧାଡ଼ି_from_source(p, lineno);
    }
    ଅନ୍ୟଥା ଯଦି (p->start_rule == Py_file_ନିବେଶ) {
        error_ଧାଡ଼ି = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) lineno, info.encoding);
    }

    ଯଦି (!error_ଧାଡ଼ି) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        ପ୍ରତିଜ୍ଞା(!info.is_file || info.status == E_EOF);

        ଯଦି (location.lineno <= lineno &&
                info.ନିବେଶ_span.end > info.ନିବେଶ_span.start) {
            Py_ssize_t size;
            ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            ଯଦି (errtype == Py_Exc_Incomplete_ନିବେଶ_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_ନିବେଶ))) {
                size--;
            }
            error_ଧାଡ଼ି = PyUnicode_DecodeUTF8(ଧାଡ଼ି, size, "replace");
        }
        ଅନ୍ୟଥା ଯଦି (!info.is_file) {
            error_ଧାଡ଼ି = get_error_ଧାଡ଼ି_from_source(p, lineno);
        }
        ଅନ୍ୟଥା {
            error_ଧାଡ଼ି = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        ଯଦି (!error_ଧାଡ଼ି) {
            ଯାଅ error;
        }
    }

    Py_ssize_t col_ସଂଖ୍ୟା = col_offset;
    Py_ssize_t end_col_ସଂଖ୍ୟା = end_col_offset;

    col_ସଂଖ୍ୟା = _PyPegen_byte_offset_to_character_offset(error_ଧାଡ଼ି, col_offset);
    ଯଦି (col_ସଂଖ୍ୟା < 0) {
        ଯାଅ error;
    }

    ଯଦି (end_col_offset > 0) {
        end_col_ସଂଖ୍ୟା = _PyPegen_byte_offset_to_character_offset(error_ଧାଡ଼ି, end_col_offset);
        ଯଦି (end_col_ସଂଖ୍ୟା < 0) {
            ଯାଅ error;
        }
    }

    tmp = Py_Build_ମୂଲ୍ୟ("(OnnNnn)", info.filename, lineno, col_ସଂଖ୍ୟା, error_ଧାଡ଼ି, end_lineno, end_col_ସଂଖ୍ୟା);
    error_ଧାଡ଼ି = ଖାଲି;
    ଯଦି (!tmp) {
        ଯାଅ error;
    }
    ମୂଲ୍ୟ = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ଯଦି (!ମୂଲ୍ୟ) {
        ଯାଅ error;
    }
    PyErr_SetObject(errtype, ମୂଲ୍ୟ);

    Py_DECREF(errstr);
    Py_DECREF(ମୂଲ୍ୟ);
    ଫେରନ୍ତୁ ଖାଲି;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_ଧାଡ଼ି);
    ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    ଯଦି (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        ଯଦି (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        ଫେରନ୍ତୁ;
    }
    // Initialization error
    ଯଦି (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    ଯଦି (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        ଯଦି (info.level) {
            raise_unclosed_parentheses_error(p);
        } ଅନ୍ୟଥା {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        ଫେରନ୍ତୁ;
    }
    // Indentation error in the tokenizer
    ଯଦି (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        ଫେରନ୍ତୁ;
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

ଶୂନ୍ୟ
Pypegen_ଗଦା_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
