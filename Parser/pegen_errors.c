#ಅನ್ತರ್ಭಾವ <Python.h>
#ಅನ್ತರ್ಭಾವ <errcode.h>

#ಅನ್ತರ್ಭಾವ "pycore_pyerrors.h"      // _PyErr_ProgramDecodedTextObject()
#ಅನ್ತರ್ಭಾವ "pycore_runtime.h"       // _Py_ID()
#ಅನ್ತರ್ಭಾವ "pycore_tuple.h"         // _PyTuple_FromPair
#ಅನ್ತರ್ಭಾವ "pegen.h"
#ಅನ್ತರ್ಭಾವ "tokenizer/tokenizer.h"

// TOKENIZER ERRORS

ಶೂನ್ಯ *
Py_Pegen_raise_incomplete_ಒಳಹರಿವು(Parser *p)
{
    ಆದರೆ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) && !PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ _PyPegen_raise_error(p, Py_Exc_Incomplete_ಒಳಹರಿವು_Error, 0,
                                    "incomplete input");
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
raise_unclosed_parentheses_error(Parser *p) {
       _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
       ಪೂರ್ಣಾಂಕ error_lineno = info.delimiter_loc.lineno;
       ಪೂರ್ಣಾಂಕ error_ಲಂಬಸಾಲು = info.delimiter_loc.byte_ಲಂಬಸಾಲು;
       RAISE_ERROR_ತಿಳಿದ_LOCATION(p, PyExc_SyntaxError,
                                  error_lineno, error_ಲಂಬಸಾಲು, error_lineno, -1,
                                  "'%c' was never closed",
                                  info.delimiter);
}

ಪೂರ್ಣಾಂಕ
_Pypegen_tokenizer_error(Parser *p)
{
    ಆದರೆ (PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = ಖಾಲಿ;
    PyObject* errtype = PyExc_SyntaxError;
    Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = -1;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    ಆದರೆ ((p->flags & Py_PARSE_ALLOW_INCOMPLETE_ಒಳಹರಿವು) &&
        (info.ಸ್ಥಿತಿ_2 == E_EOF || info.ಸ್ಥಿತಿ_2 == E_INCOMPLETE))
    {
        Py_Pegen_raise_incomplete_ಒಳಹರಿವು(p);
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆಯ್ಕೆ (info.ಸ್ಥಿತಿ_2) {
        ಸ್ಥಿತಿ E_INCOMPLETE:
            // The diagnostic pass rejected this unfinished token. Let the
            // parser report the syntax error at its original failure point.
            ಹಿಂತಿರುಗಿ -1;
        ಸ್ಥಿತಿ E_TOKEN:
            ಸಂದೇಶ = "invalid token";
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ E_EOF:
            ಆದರೆ (info.level) {
                raise_unclosed_parentheses_error(p);
            } ಇಲ್ಲದಿದ್ದರೆ {
                RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
            }
            ಹಿಂತಿರುಗಿ -1;
        ಸ್ಥಿತಿ E_DEDENT:
            RAISE_INDENTATION_ERROR("unindent does not match any outer indentation level");
            ಹಿಂತಿರುಗಿ -1;
        ಸ್ಥಿತಿ E_INTR:
            ಆದರೆ (!PyErr_Occurred()) {
                Py_Err_ಗಣ_None(Py_Exc_Keyboard_ಅಡಚಣೆ);
            }
            ಹಿಂತಿರುಗಿ -1;
        ಸ್ಥಿತಿ E_NOMEM:
            Py_Err_No_ಸ್ಮರಣೆ();
            ಹಿಂತಿರುಗಿ -1;
        ಸ್ಥಿತಿ E_TABSPACE:
            errtype = PyExc_TabError;
            ಸಂದೇಶ = "inconsistent use of tabs and spaces in indentation";
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ E_TOODEEP:
            errtype = PyExc_IndentationError;
            ಸಂದೇಶ = "too many levels of indentation";
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ E_LINECONT: {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = info.cursor - info.ಸಾಲು_ಹರವು.ಆರಂಭ - 1;
            ಸಂದೇಶ = "unexpected character after line continuation character";
            ನಿಲ್ಲಿಸು;
        }
        ಸ್ಥಿತಿ E_COLUMNOVERFLOW:
            Py_Err_ಗಣ_String(Py_Exc_ಅತಿಪ್ರವಾಹ_Error,
                    "Parser column offset overflow - source line is too big");
            ಹಿಂತಿರುಗಿ -1;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಸಂದೇಶ = "unknown parsing error";
    }

    RAISE_ERROR_ತಿಳಿದ_LOCATION(p, errtype, info.location.lineno,
                               ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ >= 0 ? ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ : 0,
                               info.location.lineno, -1, ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ -1;
}

ಪೂರ್ಣಾಂಕ
_Pypegen_raise_decode_error(Parser *p)
{
    ಪ್ರತಿಜ್ಞಾ(PyErr_Occurred());
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errtype = ಖಾಲಿ;
    ಆದರೆ (PyErr_ExceptionMatches(PyExc_UnicodeError)) {
        errtype = "unicode error";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PyErr_ExceptionMatches(Py_Exc_ಮೌಲ್ಯ_Error)) {
        errtype = "value error";
    }
    ಆದರೆ (errtype) {
        PyObject *type;
        PyObject *ಮೌಲ್ಯ;
        PyObject *tback;
        PyObject *errstr;
        PyErr_Fetch(&type, &ಮೌಲ್ಯ, &tback);
        errstr = PyObject_Str(ಮೌಲ್ಯ);
        ಆದರೆ (errstr) {
            RAISE_SYNTAX_ERROR("(%s) %U", errtype, errstr);
            Py_DECREF(errstr);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            PyErr_Clear();
            RAISE_SYNTAX_ERROR("(%s) unknown error", errtype);
        }
        Py_XDECREF(type);
        Py_XDECREF(ಮೌಲ್ಯ);
        Py_XDECREF(tback);
    }

    ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Py_Pegen_tokenize_ತುಂಬಿದ_ಮೂಲ_ಗಮ್ಯ_check_for_errors(Parser *p) {
    // Tokenize the whole input to see if there are any tokenization
    // errors such as mismatching parentheses. These will get priority
    // over generic syntax errors only if the line number of the error is
    // before the one that we had for the generic error.

    // We don't want to tokenize to the end for interactive input
    ಆದರೆ (_PyTokenizer_IsInteractive(p->tok)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    PyObject *type, *ಮೌಲ್ಯ, *traceback;
    PyErr_Fetch(&type, &ಮೌಲ್ಯ, &traceback);

    Token *current_token = p->ತಿಳಿದ_err_token != ಖಾಲಿ ? p->ತಿಳಿದ_err_token : p->ಟೋಕನುಗಳು[p->fill - 1];
    Py_ssize_t current_err_ಸಾಲು = current_token->lineno;

    ಪೂರ್ಣಾಂಕ ret = 0;
    ರಚನೆ token new_token;
    _PyToken_Init(&new_token);

    ಗಾಗಿ (;;) {
        _PyTokenizer_Get(p->tok, &new_token);
        ಆಯ್ಕೆ (new_token.type) {
            ಸ್ಥಿತಿ ERRORTOKEN: {
                ಆದರೆ (PyErr_Occurred()) {
                    ret = -1;
                    ಹೋಗು ನಿರ್ಗಮಿಸು;
                }
                _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
                ಆದರೆ (info.level != 0) {
                    ಪೂರ್ಣಾಂಕ error_lineno = info.delimiter_loc.lineno;
                    ಆದರೆ (current_err_ಸಾಲು > error_lineno) {
                        raise_unclosed_parentheses_error(p);
                        ret = -1;
                        ಹೋಗು ನಿರ್ಗಮಿಸು;
                    }
                }
                ನಿಲ್ಲಿಸು;
            }
            ಸ್ಥಿತಿ ENDMARKER:
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಮುಂದುವರಿ;
        }
        ನಿಲ್ಲಿಸು;
    }


ನಿರ್ಗಮಿಸು:
    _PyToken_Free(&new_token);
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Preserve expression errors over later formatted-string errors.
    ಆದರೆ (PyErr_Occurred() && !info.ಒಳ_formatted_string) {
        Py_XDECREF(ಮೌಲ್ಯ);
        Py_XDECREF(type);
        Py_XDECREF(traceback);
    } ಇಲ್ಲದಿದ್ದರೆ {
        PyErr_Restore(type, ಮೌಲ್ಯ, traceback);
    }
    ಹಿಂತಿರುಗಿ ret;
}

// PARSER ERRORS

ಶೂನ್ಯ *
_PyPegen_raise_error(Parser *p, PyObject *errtype, ಪೂರ್ಣಾಂಕ use_mark, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errmsg, ...)
{
    // Bail out if we already have an error set.
    ಆದರೆ (p->error_indicator && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (p->fill == 0) {
        va_list va;
        va_ಆರಂಭ(va, errmsg);
        Py_Pegen_raise_error_ತಿಳಿದ_location(p, errtype, 0, 0, 0, -1, errmsg, va);
        va_ಕೊನೆ(va);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (use_mark && p->mark == p->fill && _PyPegen_fill_token(p) < 0) {
        p->error_indicator = 1;
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    Token *t = p->ತಿಳಿದ_err_token != ಖಾಲಿ
                   ? p->ತಿಳಿದ_err_token
                   : p->ಟೋಕನುಗಳು[use_mark ? p->mark : p->fill - 1];
    Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = -1;
    ಆದರೆ (t->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == -1) {
        _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
        ಆದರೆ (info.diagnostic.location.lineno != 0) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = info.diagnostic.location.byte_ಲಂಬಸಾಲು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (info.cursor == info.ಒಳಹರಿವು_ಹರವು.ಆರಂಭ) {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = Py_ಸುರಕ್ಷಿತ_DOWNCAST(
                info.cursor - info.ಸಾಲು_ಹರವು.ಆರಂಭ, intptr_t, ಪೂರ್ಣಾಂಕ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = t->ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1;
    }

    ಆದರೆ (t->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ != -1) {
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = t->ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ + 1;
    }

    va_list va;
    va_ಆರಂಭ(va, errmsg);
    Py_Pegen_raise_error_ತಿಳಿದ_location(p, errtype, t->lineno, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, t->ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ, errmsg, va);
    va_ಕೊನೆ(va);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ PyObject *
get_error_ಸಾಲು_from_ಮೂಲ(Parser *p, Py_ssize_t lineno)
{
    ಆದರೆ (Py_Tokenizer_Retained_ಮೂಲ(p->tok) == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ Py_GetConstant(Py_CONSTANT_ಖಾಲಿಗಳು_STR);
    }
    Py_ssize_t relative_lineno = p->starting_lineno ? lineno - p->starting_lineno + 1 : lineno;
    Py_ssize_t len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = Py_Tokenizer_ಸಾಲು_View(p->tok, relative_lineno, &len);
    ಹಿಂತಿರುಗಿ PyUnicode_DecodeUTF8(ಸಾಲು, len, "replace");
}

ಶೂನ್ಯ *
Py_Pegen_raise_error_ತಿಳಿದ_location(Parser *p, PyObject *errtype,
                                    Py_ssize_t lineno, Py_ssize_t ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                    Py_ssize_t ಕೊನೆ_lineno, Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *errmsg, va_list va)
{
    // Bail out if we already have an error set.
    ಆದರೆ (p->error_indicator && PyErr_Occurred()) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    PyObject *ಮೌಲ್ಯ = ಖಾಲಿ;
    PyObject *errstr = ಖಾಲಿ;
    PyObject *error_ಸಾಲು = ಖಾಲಿ;
    PyObject *tmp = ಖಾಲಿ;
    p->error_indicator = 1;
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    _PyTok_Loc location = info.diagnostic.location.lineno != 0
        ? info.diagnostic.location : info.location;
    Py_Tok_ಹರವು ಪಠ್ಯ_ಹರವು = info.diagnostic.location.lineno != 0
        ? info.diagnostic.ಪಠ್ಯ_ಹರವು : info.ಸಾಲು_ಹರವು;

    ಆದರೆ (ಕೊನೆ_lineno == CURRENT_POS) {
        ಕೊನೆ_lineno = location.lineno;
    }
    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ == CURRENT_POS) {
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = location.byte_ಲಂಬಸಾಲು;
    }

    errstr = PyUnicode_FromFormatV(errmsg, va);
    ಆದರೆ (!errstr) {
        ಹೋಗು error;
    }

    ಆದರೆ (info.is_interactive && Py_Tokenizer_Retained_ಮೂಲ(p->tok) != ಖಾಲಿ) {
        error_ಸಾಲು = get_error_ಸಾಲು_from_ಮೂಲ(p, lineno);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->ಆರಂಭ_ನಿಯಮ == Py_file_ಒಳಹರಿವು) {
        error_ಸಾಲು = Py_Err_ಕಾರ್ಯಕ್ರಮ_Decoded_ಪಠ್ಯ_Object(info.filename,
                                                     (ಪೂರ್ಣಾಂಕ) lineno, info.encoding);
    }

    ಆದರೆ (!error_ಸಾಲು) {
        /* PyErr_ProgramTextObject was not called or returned NULL. If it was not called,
           then we need to find the error line from some other source, because
           p->start_rule != Py_file_input. If it returned NULL, then it either unexpectedly
           failed or we're parsing from a string or the REPL. There's a third edge case where
           we're actually parsing from a file, which has an E_EOF SyntaxError and in that case
           `PyErr_ProgramTextObject` fails because lineno points to last_file_line + 1, which
           does not physically exist */
        ಪ್ರತಿಜ್ಞಾ(!info.is_file || info.ಸ್ಥಿತಿ_2 == E_EOF);

        ಆದರೆ (location.lineno <= lineno &&
                info.ಒಳಹರಿವು_ಹರವು.ಕೊನೆ > info.ಒಳಹರಿವು_ಹರವು.ಆರಂಭ) {
            Py_ssize_t ಗಾತ್ರ_2;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = Py_Tokenizer_ಹರವು_View(
                p->tok, ಪಠ್ಯ_ಹರವು, &ಗಾತ್ರ_2);
            ಆದರೆ (errtype == Py_Exc_Incomplete_ಒಳಹರಿವು_Error && ಗಾತ್ರ_2 > 0 &&
                (info.implicit_newline == _PyTokenizer_APPEND_NEWLINE ||
                 (info.implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
                  p->ಆರಂಭ_ನಿಯಮ != Py_file_ಒಳಹರಿವು))) {
                ಗಾತ್ರ_2--;
            }
            error_ಸಾಲು = PyUnicode_DecodeUTF8(ಸಾಲು, ಗಾತ್ರ_2, "replace");
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!info.is_file) {
            error_ಸಾಲು = get_error_ಸಾಲು_from_ಮೂಲ(p, lineno);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            error_ಸಾಲು = Py_GetConstant(Py_CONSTANT_ಖಾಲಿಗಳು_STR);
        }
        ಆದರೆ (!error_ಸಾಲು) {
            ಹೋಗು error;
        }
    }

    Py_ssize_t ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    Py_ssize_t ಕೊನೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;

    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ(error_ಸಾಲು, ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0) {
        ಹೋಗು error;
    }

    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ > 0) {
        ಕೊನೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = Py_Pegen_byte_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_character_ಸ್ಥಾನಾಂತರ(error_ಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ);
        ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0) {
            ಹೋಗು error;
        }
    }

    tmp = Py_Build_ಮೌಲ್ಯ("(OnnNnn)", info.filename, lineno, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, error_ಸಾಲು, ಕೊನೆ_lineno, ಕೊನೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    error_ಸಾಲು = ಖಾಲಿ;
    ಆದರೆ (!tmp) {
        ಹೋಗು error;
    }
    ಮೌಲ್ಯ = _PyTuple_FromPair(errstr, tmp);
    Py_DECREF(tmp);
    ಆದರೆ (!ಮೌಲ್ಯ) {
        ಹೋಗು error;
    }
    Py_Err_ಗಣ_Object(errtype, ಮೌಲ್ಯ);

    Py_DECREF(errstr);
    Py_DECREF(ಮೌಲ್ಯ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

error:
    Py_XDECREF(errstr);
    Py_XDECREF(error_ಸಾಲು);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ
Pypegen_ಗಣ_syntax_error(Parser* p, Token* ಕೊನೆಯ_token) {
    _PyTokenizer_Info info = _PyTokenizer_GetInfo(p->tok);
    // Existing syntax error
    ಆದರೆ (PyErr_Occurred()) {
        // Prioritize tokenizer errors to custom syntax errors raised
        // on the second phase only if the errors come from the parser.
        ಪೂರ್ಣಾಂಕ is_tok_ok = (info.ಸ್ಥಿತಿ_2 == E_ಮುಗಿದ || info.ಸ್ಥಿತಿ_2 == E_OK);
        ಆದರೆ (is_tok_ok && PyErr_ExceptionMatches(PyExc_SyntaxError)) {
            Py_Pegen_tokenize_ತುಂಬಿದ_ಮೂಲ_ಗಮ್ಯ_check_for_errors(p);
        }
        // Propagate the existing syntax error.
        ಹಿಂತಿರುಗಿ;
    }
    // Initialization error
    ಆದರೆ (p->fill == 0) {
        RAISE_SYNTAX_ERROR("error at start before reading any input");
    }
    // Parser encountered EOF (End of File) unexpectedtly
    ಆದರೆ (ಕೊನೆಯ_token->type == ERRORTOKEN && info.ಸ್ಥಿತಿ_2 == E_EOF) {
        ಆದರೆ (info.level) {
            raise_unclosed_parentheses_error(p);
        } ಇಲ್ಲದಿದ್ದರೆ {
            RAISE_SYNTAX_ERROR("unexpected EOF while parsing");
        }
        ಹಿಂತಿರುಗಿ;
    }
    // Indentation error in the tokenizer
    ಆದರೆ (ಕೊನೆಯ_token->type == INDENT || ಕೊನೆಯ_token->type == DEDENT) {
        RAISE_INDENTATION_ERROR(ಕೊನೆಯ_token->type == INDENT ? "unexpected indent" : "unexpected unindent");
        ಹಿಂತಿರುಗಿ;
    }
    // Unknown error (generic case)

    // Use the last token we found on the first pass to avoid reporting
    // incorrect locations for generic syntax errors just because we reached
    // further away when trying to find specific syntax errors in the second
    // pass.
    RAISE_SYNTAX_ERROR_ತಿಳಿದ_LOCATION(ಕೊನೆಯ_token, "invalid syntax");
    // _PyPegen_tokenize_full_source_to_check_for_errors will override the existing
    // generic SyntaxError we just raised if errors are found.
    Py_Pegen_tokenize_ತುಂಬಿದ_ಮೂಲ_ಗಮ್ಯ_check_for_errors(p);
}

ಶೂನ್ಯ
Pypegen_ರಾಶಿ_ಅತಿಪ್ರವಾಹ(Parser *p)
{
    p->error_indicator = 1;
    Py_Err_ಗಣ_String(Py_Exc_ಸ್ಮರಣೆ_Error,
        "Parser stack overflowed - Python source too complex to parse");
}
