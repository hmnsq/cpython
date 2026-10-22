#అన్తర్భావ <Python.h>
#అన్తర్భావ <errcode.h>

#అన్తర్భావ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#అన్తర్భావ "pycore_runtime.h"       // _Py_ID()
#అన్తర్భావ "pycore_tuple.h"         // _PyTuple_FromPair
#అన్తర్భావ "pegen.h"
#అన్తర్భావ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

శూన్య *
Py_Pegen_raise_incomplete_ఆదానం(Parser *p)
{
    ఒకవేళ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం) && !PyErr_Occurred()) {
        తిరిగి _PyPegen_raise_error(p, Py_Exc_Incomplete_ఆదానం_Error, 0,
                                    "incomplete input");
    }
    తిరిగి ఖాళీ;
}

స్థిర అంతఃవరుస శూన్య
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       పూర్ణాంకం error_lineno = info.delimiter_loc.lineno;
       పూర్ణాంకం error_నిలువు = info.delimiter_loc.byte_నిలువు;
       RAISE_ERROR_తెలిసిన_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_నిలువు, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

పూర్ణాంకం
_Pypegen_tokenizer_error(Parser *p)
{
    ఒకవేళ (PyErr_Occurred()) {
        తిరిగి -1;
    }

    స్థిరరాశి అక్షరం *సందేశం = ఖాళీ;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t నిలువు_స్థానభ్రంశం = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ఒకవేళ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ఆదానం) &&
        (info.స్థితి_2 == E_EOF || info.స్థితి_2 == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_ఆదానం(p);
        తిరిగి -1;
    }
    ఎంపిక (info.స్థితి_2) {
        స్థితి E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            తిరిగి -1;
        స్థితి E_TOKEN:
            సందేశం = "invalid token";
            ఆపు;
        స్థితి E_EOF:
            ఒకవేళ (info.level) {
                raise_unclosed_parentheses_error(p);
            } లేకపోతే {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            తిరిగి -1;
        స్థితి E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            తిరిగి -1;
        స్థితి E_INTR:
            ఒకవేళ (!PyErr_Occurred()) {
                Py_Err_సమితి_None(Py_Exc_Keyboard_అంతరాయం);
            }
            తిరిగి -1;
        స్థితి E_NOMEM:
            Py_Err_No_జ్ఞాపకం();
            తిరిగి -1;
        స్థితి E_TABSPACE:
            errtype = PyExc_TabError;
            సందేశం = "inconsistent use of tabs and spaces in indentation";
            ఆపు;
        స్థితి E_TOODEEP:
            errtype = PyExc_IndentationError;
            సందేశం = "too many levels of indentation";
            ఆపు;
        స్థితి E_LINECONT: {
            నిలువు_స్థానభ్రంశం = info.cursor - info.పంక్తి_పరిధి.ప్రారంభం - 1;
            సందేశం = "unexpected character after line continuation character";
            ఆపు;
        }
        స్థితి E_COLUMNOVERFLOW:
            Py_Err_సమితి_String(Py_Exc_అధికప్రవాహం_Error,
                    "Parser column offset overflow - source line is too big");
            తిరిగి -1;
        అప్రమేయ:
            సందేశం = "unknown parsing error";
    }

    RAISE_ERROR_తెలిసిన_LOCATION(p, errtype, info.location.lineno,
                               నిలువు_స్థానభ్రంశం >= 0 ? నిలువు_స్థానభ్రంశం : 0,
                               info.location.lineno, -1, సందేశం);
    తిరిగి -1;
}

పూర్ణాంకం
_Pypegen_raise_decode_error(Parser *p)
{
    ప్రతిజ్ఞా(PyErr_Occurred());
    స్థిరరాశి అక్షరం *errtype = ఖాళీ;
    ఒకవేళ (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    లేకపోతే ఒకవేళ (PyErr_ExceptionMatches(Py_Exc_విలువ_Error)) {
        errtype = "value error";
    }
    ఒకవేళ (errtype) {
        PyObject *type;
        PyObject *విలువ;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &విలువ, &tback);
        errstr = PyObject_Str(విలువ);
        ఒకవేళ (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        లేకపోతే {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(విలువ);
        Py_XDECREF(tback);
    }

    తిరిగి -1;
}

స్థిర పూర్ణాంకం
Py_Pegen_tokenize_నిండిన_ఆధారం_గమ్యం_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    ఒకవేళ (_PyTokenizer_IsInteractive(p->tok)) {
        తిరిగి 0;
    }

    PyObject *type, *విలువ, *traceback;
    PyErr_Fetch(&type, &విలువ, &traceback);

    Token *current_token = p->తెలిసిన_err_token != ఖాళీ ? p->తెలిసిన_err_token : p->టోకెన్లు[p->fill - 1];
    Py_ssize_t current_err_పంక్తి = current_token->lineno;

    పూర్ణాంకం ret = 0;
    నిర్మాణం token new_token;
    _PyToken_Init(&new_token);

    కొరకు (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        ఎంపిక (new_token.type) {
            స్థితి ERRORTOKEN: {
                ఒకవేళ (PyErr_Occurred()) {
                    ret = -1;
                    వెళ్ళు నిష్క్రమించు;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                ఒకవేళ (info.level != 0) {
                    పూర్ణాంకం error_lineno = info.delimiter_loc.lineno;
                    ఒకవేళ (current_err_పంక్తి > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        వెళ్ళు నిష్క్రమించు;
                    }
                }
                ఆపు;
            }
            స్థితి ENDMARKER:
                ఆపు;
            అప్రమేయ:
                కొనసాగు;
        }
        ఆపు;
    }


నిష్క్రమించు:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    ఒకవేళ (PyErr_Occurred() && !info.లోపల_formatted_string) {
        Py_XDECREF(విలువ);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } లేకపోతే {
        PyErr_Restore(type, విలువ, traceback);
    }
    తిరిగి ret;
}

// PARSER ERRORS

శూన్య *
_PyPegen_raise_error(Parser *p, PyObject *errtype, పూర్ణాంకం use_mark, స్థిరరాశి అక్షరం *errmsg, ...)
{
    // Bail out if we already have an error set.
    ఒకవేళ (p->error_indicator && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (p->fill == 0) {
        va_list va;
        va_ప్రారంభం(va, errmsg);
        Py_Pegen_raise_error_తెలిసిన_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_చివర(va);
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        తిరిగి ఖాళీ;
    }
    Token *t = p->తెలిసిన_err_token != ఖాళీ
                   ? p->తెలిసిన_err_token
                   : p->టోకెన్లు[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t నిలువు_స్థానభ్రంశం;
    Py_ssize_t చివర_నిలువు_స్థానభ్రంశం = -1;
    ఒకవేళ (t->నిలువు_స్థానభ్రంశం == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        ఒకవేళ (info.diagnostic.location.lineno != 0) {
            నిలువు_స్థానభ్రంశం = info.diagnostic.location.byte_నిలువు;
        } లేకపోతే ఒకవేళ (info.cursor == info.ఆదానం_పరిధి.ప్రారంభం) {
            నిలువు_స్థానభ్రంశం = 0;
        } లేకపోతే {
            నిలువు_స్థానభ్రంశం = Py_సురక్షిత_DOWNCAST(
                info.cursor - info.పంక్తి_పరిధి.ప్రారంభం, intptr_t, పూర్ణాంకం);
        }
    } లేకపోతే {
        నిలువు_స్థానభ్రంశం = t->నిలువు_స్థానభ్రంశం + 1;
    }

    ఒకవేళ (t->చివర_నిలువు_స్థానభ్రంశం != -1) {
        చివర_నిలువు_స్థానభ్రంశం = t->చివర_నిలువు_స్థానభ్రంశం + 1;
    }

    va_list va;
    va_ప్రారంభం(va, errmsg);
    Py_Pegen_raise_error_తెలిసిన_location(p, errtype, t->lineno, నిలువు_స్థానభ్రంశం, t->చివర_lineno, చివర_నిలువు_స్థానభ్రంశం, errmsg, va);
    va_చివర(va);

    తిరిగి ఖాళీ;
}

స్థిర PyObject *
get_error_పంక్తి_from_ఆధారం(Parser *p, Py_ssize_t lineno)
{
    ఒకవేళ (Py_Tokenizer_Retained_ఆధారం(p->tok) == ఖాళీ) {
        తిరిగి Py_GetConstant(Py_CONSTANT_ఖాళీలు_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    స్థిరరాశి అక్షరం *పంక్తి = Py_Tokenizer_పంక్తి_View(p->tok, relative_lineno, &len);
    తిరిగి PyUnicode_DecodeUTF8(పంక్తి, len, "replace");
}

శూన్య *
Py_Pegen_raise_error_తెలిసిన_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t నిలువు_స్థానభ్రంశం,
                                    Py_ssize_t చివర_lineno, Py_ssize_t చివర_నిలువు_స్థానభ్రంశం,
                                    స్థిరరాశి అక్షరం *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    ఒకవేళ (p->error_indicator && PyErr_Occurred()) {
        తిరిగి ఖాళీ;
    }
    PyObject *విలువ = ఖాళీ;
    PyObject *errstr = ఖాళీ;
    PyObject *error_పంక్తి = ఖాళీ;
    PyObject *tmp = ఖాళీ;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_పరిధి పాఠ్యం_పరిధి = info.diagnostic.location.lineno != 0
        ? info.diagnostic.పాఠ్యం_పరిధి : info.పంక్తి_పరిధి;

    ఒకవేళ (చివర_lineno == CURRENT_POS) {
        చివర_lineno = location.lineno;
    }
    ఒకవేళ (చివర_నిలువు_స్థానభ్రంశం == CURRENT_POS) {
        చివర_నిలువు_స్థానభ్రంశం = location.byte_నిలువు;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    ఒకవేళ (!errstr) {
        వెళ్ళు error;
    }

    ఒకవేళ (info.is_interactive && Py_Tokenizer_Retained_ఆధారం(p->tok) != ఖాళీ) {
        error_పంక్తి = get_error_పంక్తి_from_ఆధారం(p, lineno);
    }
    లేకపోతే ఒకవేళ (p->ప్రారంభం_నియమం == Py_file_ఆదానం) {
        error_పంక్తి = Py_Err_కార్యక్రమం_Decoded_పాఠ్యం_Object(info.filename,
                                                     (పూర్ణాంకం) lineno, info.encoding);
    }

    ఒకవేళ (!error_పంక్తి) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        ప్రతిజ్ఞా(!info.is_file || info.స్థితి_2 == E_EOF);

        ఒకవేళ (location.lineno <= lineno &&
                info.ఆదానం_పరిధి.చివర > info.ఆదానం_పరిధి.ప్రారంభం) {
            Py_ssize_t కొలత;
            స్థిరరాశి అక్షరం *పంక్తి = Py_Tokenizer_పరిధి_View(
                p->tok, పాఠ్యం_పరిధి, &కొలత);
            ఒకవేళ (errtype == Py_Exc_Incomplete_ఆదానం_Error && కొలత > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->ప్రారంభం_నియమం != Py_file_ఆదానం))) {
                కొలత--;
            }
            error_పంక్తి = PyUnicode_DecodeUTF8(పంక్తి, కొలత, "replace");
        }
        లేకపోతే ఒకవేళ (!info.is_file) {
            error_పంక్తి = get_error_పంక్తి_from_ఆధారం(p, lineno);
        }
        లేకపోతే {
            error_పంక్తి = Py_GetConstant(Py_CONSTANT_ఖాళీలు_STR);
        }
        ఒకవేళ (!error_పంక్తి) {
            వెళ్ళు error;
        }
    }

    Py_ssize_t నిలువు_సంఖ్య = నిలువు_స్థానభ్రంశం;
    Py_ssize_t చివర_నిలువు_సంఖ్య = చివర_నిలువు_స్థానభ్రంశం;

    నిలువు_సంఖ్య = Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం(error_పంక్తి, నిలువు_స్థానభ్రంశం);
    ఒకవేళ (నిలువు_సంఖ్య < 0) {
        వెళ్ళు error;
    }

    ఒకవేళ (చివర_నిలువు_స్థానభ్రంశం > 0) {
        చివర_నిలువు_సంఖ్య = Py_Pegen_byte_స్థానభ్రంశం_గమ్యం_character_స్థానభ్రంశం(error_పంక్తి, చివర_నిలువు_స్థానభ్రంశం);
        ఒకవేళ (చివర_నిలువు_సంఖ్య < 0) {
            వెళ్ళు error;
        }
    }

    tmp = Py_Build_విలువ("(OnnNnn)", info.filename, lineno, నిలువు_సంఖ్య, error_పంక్తి, చివర_lineno, చివర_నిలువు_సంఖ్య);
    error_పంక్తి = ఖాళీ;
    ఒకవేళ (!tmp) {
        వెళ్ళు error;
    }
    విలువ = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ఒకవేళ (!విలువ) {
        వెళ్ళు error;
    }
    Py_Err_సమితి_Object(errtype, విలువ);

    Py_DECREF(errstr);
    Py_DECREF(విలువ);
    తిరిగి ఖాళీ;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_పంక్తి);
    తిరిగి ఖాళీ;
}

శూన్య
Pypegen_సమితి_syntax_error(Parser* p, Token* చివరి_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    ఒకవేళ (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        పూర్ణాంకం is_tok_ok = (info.స్థితి_2 == E_పూర్తి || info.స్థితి_2 == E_OK);
        ఒకవేళ (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_నిండిన_ఆధారం_గమ్యం_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        తిరిగి;
    }
    // Initialization error
    ఒకవేళ (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    ఒకవేళ (చివరి_token->type == ERRORTOKEN && info.స్థితి_2 == E_EOF) {
        ఒకవేళ (info.level) {
            raise_unclosed_parentheses_error(p);
        } లేకపోతే {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        తిరిగి;
    }
    // Indentation error in the tokenizer
    ఒకవేళ (చివరి_token->type == INDENT || చివరి_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(చివరి_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        తిరిగి;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_తెలిసిన_LOCATION(చివరి_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_నిండిన_ఆధారం_గమ్యం_check_for_errors(p);
}

శూన్య
Pypegen_దొంతర_అధికప్రవాహం(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_సమితి_String(Py_Exc_జ్ఞాపకం_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
