#அந்தர்பாவ <Python.h>
#அந்தர்பாவ <errcode.h>

#அந்தர்பாவ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#அந்தர்பாவ "pycore_runtime.h"       // _Py_ID()
#அந்தர்பாவ "pycore_tuple.h"         // _PyTuple_FromPair
#அந்தர்பாவ "pegen.h"
#அந்தர்பாவ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

வெறுமை *
Py_Pegen_raise_incomplete_உள்ளீடு(Parser *p)
{
    எனில் ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு) && !PyErr_Occurred()) {
        திரும்பு _PyPegen_raise_error(p, Py_Exc_Incomplete_உள்ளீடு_Error, 0,
                                    "incomplete input");
    }
    திரும்பு வெற்று;
}

நிலையான உள்வரி வெறுமை
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       முழு error_lineno = info.delimiter_loc.lineno;
       முழு error_col = info.delimiter_loc.byte_col;
       RAISE_ERROR_KNOWN_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_col, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

முழு
_Pypegen_tokenizer_error(Parser *p)
{
    எனில் (PyErr_Occurred()) {
        திரும்பு -1;
    }

    மாறிலி எழுத்து *செய்தி = வெற்று;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t col_offset = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    எனில் ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_உள்ளீடு) &&
        (info.status == E_EOF || info.status == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_உள்ளீடு(p);
        திரும்பு -1;
    }
    தேர்வு (info.status) {
        நிலை E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            திரும்பு -1;
        நிலை E_TOKEN:
            செய்தி = "invalid token";
            நிறுத்து;
        நிலை E_EOF:
            எனில் (info.level) {
                raise_unclosed_parentheses_error(p);
            } இல்லையெனில் {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            திரும்பு -1;
        நிலை E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            திரும்பு -1;
        நிலை E_INTR:
            எனில் (!PyErr_Occurred()) {
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            }
            திரும்பு -1;
        நிலை E_NOMEM:
            PyErr_NoMemory();
            திரும்பு -1;
        நிலை E_TABSPACE:
            errtype = PyExc_TabError;
            செய்தி = "inconsistent use of tabs and spaces in indentation";
            நிறுத்து;
        நிலை E_TOODEEP:
            errtype = PyExc_IndentationError;
            செய்தி = "too many levels of indentation";
            நிறுத்து;
        நிலை E_LINECONT: {
            col_offset = info.cursor - info.வரி_span.start - 1;
            செய்தி = "unexpected character after line continuation character";
            நிறுத்து;
        }
        நிலை E_COLUMNOVERFLOW:
            PyErr_SetString(PyExc_OverflowError,
                    "Parser column offset overflow - source line is too big");
            திரும்பு -1;
        இயல்பு:
            செய்தி = "unknown parsing error";
    }

    RAISE_ERROR_KNOWN_LOCATION(p, errtype, info.location.lineno,
                               col_offset >= 0 ? col_offset : 0,
                               info.location.lineno, -1, செய்தி);
    திரும்பு -1;
}

முழு
_Pypegen_raise_decode_error(Parser *p)
{
    ப்ரதிஜ்ஞா(PyErr_Occurred());
    மாறிலி எழுத்து *errtype = வெற்று;
    எனில் (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    இல்லையெனில் எனில் (PyErr_ExceptionMatches(Py_Exc_மதிப்பு_Error)) {
        errtype = "value error";
    }
    எனில் (errtype) {
        PyObject *type;
        PyObject *மதிப்பு;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &மதிப்பு, &tback);
        errstr = PyObject_Str(மதிப்பு);
        எனில் (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        இல்லையெனில் {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(மதிப்பு);
        Py_XDECREF(tback);
    }

    திரும்பு -1;
}

நிலையான முழு
_PyPegen_tokenize_full_source_to_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    எனில் (_PyTokenizer_IsInteractive(p->tok)) {
        திரும்பு 0;
    }

    PyObject *type, *மதிப்பு, *traceback;
    PyErr_Fetch(&type, &மதிப்பு, &traceback);

    Token *current_token = p->known_err_token != வெற்று ? p->known_err_token : p->tokens[p->fill - 1];
    Py_ssize_t current_err_வரி = current_token->lineno;

    முழு ret = 0;
    கட்டமைப்பு token new_token;
    _PyToken_Init(&new_token);

    க்கு (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        தேர்வு (new_token.type) {
            நிலை ERRORTOKEN: {
                எனில் (PyErr_Occurred()) {
                    ret = -1;
                    செல் exit;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                எனில் (info.level != 0) {
                    முழு error_lineno = info.delimiter_loc.lineno;
                    எனில் (current_err_வரி > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        செல் exit;
                    }
                }
                நிறுத்து;
            }
            நிலை ENDMARKER:
                நிறுத்து;
            இயல்பு:
                தொடர்;
        }
        நிறுத்து;
    }


exit:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    எனில் (PyErr_Occurred() && !info.in_formatted_string) {
        Py_XDECREF(மதிப்பு);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } இல்லையெனில் {
        PyErr_Restore(type, மதிப்பு, traceback);
    }
    திரும்பு ret;
}

// PARSER ERRORS

வெறுமை *
_PyPegen_raise_error(Parser *p, PyObject *errtype, முழு use_mark, மாறிலி எழுத்து *errmsg, ...)
{
    // Bail out if we already have an error set.
    எனில் (p->error_indicator && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }
    எனில் (p->fill == 0) {
        va_list va;
        va_start(va, errmsg);
        _PyPegen_raise_error_known_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_end(va);
        திரும்பு வெற்று;
    }
    எனில் (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        திரும்பு வெற்று;
    }
    Token *t = p->known_err_token != வெற்று
                   ? p->known_err_token
                   : p->tokens[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t col_offset;
    Py_ssize_t end_col_offset = -1;
    எனில் (t->col_offset == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        எனில் (info.diagnostic.location.lineno != 0) {
            col_offset = info.diagnostic.location.byte_col;
        } இல்லையெனில் எனில் (info.cursor == info.உள்ளீடு_span.start) {
            col_offset = 0;
        } இல்லையெனில் {
            col_offset = Py_SAFE_DOWNCAST(
                info.cursor - info.வரி_span.start, intptr_t, முழு);
        }
    } இல்லையெனில் {
        col_offset = t->col_offset + 1;
    }

    எனில் (t->end_col_offset != -1) {
        end_col_offset = t->end_col_offset + 1;
    }

    va_list va;
    va_start(va, errmsg);
    _PyPegen_raise_error_known_location(p, errtype, t->lineno, col_offset, t->end_lineno, end_col_offset, errmsg, va);
    va_end(va);

    திரும்பு வெற்று;
}

நிலையான PyObject *
get_error_வரி_from_source(Parser *p, Py_ssize_t lineno)
{
    எனில் (_PyTokenizer_RetainedSource(p->tok) == வெற்று) {
        திரும்பு Py_GetConstant(Py_CONSTANT_EMPTY_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    மாறிலி எழுத்து *வரி = Py_Tokenizer_வரி_View(p->tok, relative_lineno, &len);
    திரும்பு PyUnicode_DecodeUTF8(வரி, len, "replace");
}

வெறுமை *
_PyPegen_raise_error_known_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t col_offset,
                                    Py_ssize_t end_lineno, Py_ssize_t end_col_offset,
                                    மாறிலி எழுத்து *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    எனில் (p->error_indicator && PyErr_Occurred()) {
        திரும்பு வெற்று;
    }
    PyObject *மதிப்பு = வெற்று;
    PyObject *errstr = வெற்று;
    PyObject *error_வரி = வெற்று;
    PyObject *tmp = வெற்று;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    _PyTok_Span text_span = info.diagnostic.location.lineno != 0
        ? info.diagnostic.text_span : info.வரி_span;

    எனில் (end_lineno == CURRENT_POS) {
        end_lineno = location.lineno;
    }
    எனில் (end_col_offset == CURRENT_POS) {
        end_col_offset = location.byte_col;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    எனில் (!errstr) {
        செல் error;
    }

    எனில் (info.is_interactive && _PyTokenizer_RetainedSource(p->tok) != வெற்று) {
        error_வரி = get_error_வரி_from_source(p, lineno);
    }
    இல்லையெனில் எனில் (p->start_rule == Py_file_உள்ளீடு) {
        error_வரி = _PyErr_ProgramDecodedTextObject(info.filename,
                                                     (முழு) lineno, info.encoding);
    }

    எனில் (!error_வரி) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        ப்ரதிஜ்ஞா(!info.is_file || info.status == E_EOF);

        எனில் (location.lineno <= lineno &&
                info.உள்ளீடு_span.end > info.உள்ளீடு_span.start) {
            Py_ssize_t size;
            மாறிலி எழுத்து *வரி = _PyTokenizer_SpanView(
                p->tok, text_span, &size);
            எனில் (errtype == Py_Exc_Incomplete_உள்ளீடு_Error && size > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->start_rule != Py_file_உள்ளீடு))) {
                size--;
            }
            error_வரி = PyUnicode_DecodeUTF8(வரி, size, "replace");
        }
        இல்லையெனில் எனில் (!info.is_file) {
            error_வரி = get_error_வரி_from_source(p, lineno);
        }
        இல்லையெனில் {
            error_வரி = Py_GetConstant(Py_CONSTANT_EMPTY_STR);
        }
        எனில் (!error_வரி) {
            செல் error;
        }
    }

    Py_ssize_t col_எண் = col_offset;
    Py_ssize_t end_col_எண் = end_col_offset;

    col_எண் = _PyPegen_byte_offset_to_character_offset(error_வரி, col_offset);
    எனில் (col_எண் < 0) {
        செல் error;
    }

    எனில் (end_col_offset > 0) {
        end_col_எண் = _PyPegen_byte_offset_to_character_offset(error_வரி, end_col_offset);
        எனில் (end_col_எண் < 0) {
            செல் error;
        }
    }

    tmp = Py_Build_மதிப்பு("(OnnNnn)", info.filename, lineno, col_எண், error_வரி, end_lineno, end_col_எண்);
    error_வரி = வெற்று;
    எனில் (!tmp) {
        செல் error;
    }
    மதிப்பு = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    எனில் (!மதிப்பு) {
        செல் error;
    }
    PyErr_SetObject(errtype, மதிப்பு);

    Py_DECREF(errstr);
    Py_DECREF(மதிப்பு);
    திரும்பு வெற்று;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_வரி);
    திரும்பு வெற்று;
}

வெறுமை
_Pypegen_set_syntax_error(Parser* p, Token* last_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    எனில் (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        முழு is_tok_ok = (info.status == E_DONE || info.status == E_OK);
        எனில் (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            _PyPegen_tokenize_full_source_to_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        திரும்பு;
    }
    // Initialization error
    எனில் (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    எனில் (last_token->type == ERRORTOKEN && info.status == E_EOF) {
        எனில் (info.level) {
            raise_unclosed_parentheses_error(p);
        } இல்லையெனில் {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        திரும்பு;
    }
    // Indentation error in the tokenizer
    எனில் (last_token->type == INDENT || last_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(last_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        திரும்பு;
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

வெறுமை
Pypegen_அடுக்கு_overflow(Parser *p)
{
    p->error_indicator = 1;
    PyErr_SetString(PyExc_MemoryError,
        "Parser stack overflowed - Python source too complex to parse");
}
