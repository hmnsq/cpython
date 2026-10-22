#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "tokenizer.h"
#ಅನ್ತರ್ಭಾವ "reader.h"
#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

ಪೂರ್ಣಾಂಕ
Py_Tokenizer_Get_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಹಿಂತಿರುಗಿ tok->ಮುಗಿದ;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_GetNewlineRequirements(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಹಿಂತಿರುಗಿ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    _PyTokenizer_Info info = {
        .ಸ್ಥಿತಿ_2 = tok->ಮುಗಿದ,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->ಸಾಲು_ಆರಂಭ < 0
            ? -1 : (ಪೂರ್ಣಾಂಕ)(tok->cur - tok->ಸಾಲು_ಆರಂಭ)},
        .cursor = tok->cur,
        .ಒಳಹರಿವು_ಹರವು = {tok->buf_ಸ್ಥಾನಾಂತರ, tok->inp},
        .ಸಾಲು_ಹರವು = {tok->ಸಾಲು_ಆರಂಭ, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .ಒಳ_formatted_string = tok->ftstring_ಆಳ != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ಖಾಲಿ && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    ಆದರೆ (tok->level > 0) {
        ಪೂರ್ಣಾಂಕ level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ಹಿಂತಿರುಗಿ info;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Token_ಪಠ್ಯ_View(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ರಚನೆ token *token,
                  Py_ssize_t *ಉದ್ದಳತೆ)
{
    ಪ್ರತಿಜ್ಞಾ(ಉದ್ದಳತೆ != ಖಾಲಿ);
    ಆದರೆ (token->ಹರವು.ಆರಂಭ < 0) {
        ಪ್ರತಿಜ್ಞಾ(token->ಹರವು.ಆರಂಭ == -1 && token->ಹರವು.ಕೊನೆ == -1);
        *ಉದ್ದಳತೆ = 0;
        ಹಿಂತಿರುಗಿ "";
    }
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ಹರವು_View(&tok->ಮೂಲ, token->ಹರವು, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tokenizer_ಹರವು_View(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok, Py_Tok_ಹರವು ಹರವು,
                      Py_ssize_t *ಉದ್ದಳತೆ)
{
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ಹರವು_View(&tok->ಮೂಲ, ಹರವು, ಉದ್ದಳತೆ);
}

ಶೂನ್ಯ
_PyToken_GetView(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok, ಸ್ಥಿರಾಂಕ ರಚನೆ token *token,
                 _PyToken_View *view)
{
    ಪ್ರತಿಜ್ಞಾ(view != ಖಾಲಿ);
    view->ಪಠ್ಯ = Py_Token_ಪಠ್ಯ_View(tok, token, &view->ಉದ್ದಳತೆ);
    view->ಕೊನೆ_ಸಾಲು_ಆರಂಭ = tok->ಸಾಲು_ಆರಂಭ;
    view->ಸಾಲು_ಹರವು = (Py_Tok_ಹರವು){
        ISSTRINGLIT(token->type)
            ? token->ಹರವು.ಆರಂಭ - token->ಆರಂಭ_loc.byte_ಲಂಬಸಾಲು : tok->ಸಾಲು_ಆರಂಭ,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->ಮುಗಿದ == E_EOF;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Py_Tokenizer_ಸಾಲು_View(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *ಉದ್ದಳತೆ)
{
    ಹಿಂತಿರುಗಿ Py_Tok_ಮೂಲ_ಸಾಲು_View(&tok->ಮೂಲ, lineno, ಉದ್ದಳತೆ);
}

ಶೂನ್ಯ
Py_Tokenizer_ಗಣ_Context(ರಚನೆ tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

ಶೂನ್ಯ
Py_Tokenizer_ಗಣ_Options(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ extra_ಟೋಕನುಗಳು,
                        ಪೂರ್ಣಾಂಕ type_comments)
{
    tok->tok_extra_ಟೋಕನುಗಳು = extra_ಟೋಕನುಗಳು;
    tok->type_comments = type_comments;
}

ಶೂನ್ಯ
_PyTokenizer_ImplyDedents(ರಚನೆ tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

ಶೂನ್ಯ
Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(ರಚನೆ tok_state *tok, ಪೂರ್ಣಾಂಕ allow)
{
    tok->allow_incomplete_ಒಳಹರಿವು = allow != 0;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_HasTrailingStatement(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off ಕೊನೆ = tok->ಮೂಲ.ತಳ_ಸ್ಥಾನಾಂತರ + tok->ಮೂಲ.len;
    ಆಗ (cur < ಕೊನೆ) {
        ಪೂರ್ಣಾಂಕ c = Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, cur++);
        ಆದರೆ (c == '\0') {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (c != '#') {
            ಹಿಂತಿರುಗಿ 1;
        }
        ಆಗ (cur < ಕೊನೆ) {
            c = Py_Tok_ಮೂಲ_Byte(&tok->ಮೂಲ, cur);
            ಆದರೆ (c == '\0' || c == '\n') {
                ನಿಲ್ಲಿಸು;
            }
            cur++;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
_PyTokenizer_IsInteractive(ಸ್ಥಿರಾಂಕ ರಚನೆ tok_state *tok)
{
    ಹಿಂತಿರುಗಿ _PyTok_ReaderIsInteractive(tok);
}

ಶೂನ್ಯ
_PyTokenizer_StopInteractive(ರಚನೆ tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
