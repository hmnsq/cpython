#ਅਨ੍ਤਰ੍ਭਾਵ "Python.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pycore_token.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "tokenizer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "reader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "reader_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../lexer/state.h"

ਪੂਰਨ
Py_Tokenizer_Get_ਸਥਿਤੀ(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਵਾਪਸ tok->ਪੂਰਾ;
}

ਪੂਰਨ
_PyTokenizer_GetNewlineRequirements(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਵਾਪਸ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    _PyTokenizer_Info info = {
        .ਸਥਿਤੀ = tok->ਪੂਰਾ,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->ਸਤਰ_ਸ਼ੁਰੂ < 0
            ? -1 : (ਪੂਰਨ)(tok->cur - tok->ਸਤਰ_ਸ਼ੁਰੂ)},
        .cursor = tok->cur,
        .ਨਿਵੇਸ਼_ਵਿਸਤਾਰ = {tok->buf_ਵਿੱਥ, tok->inp},
        .ਸਤਰ_ਵਿਸਤਾਰ = {tok->ਸਤਰ_ਸ਼ੁਰੂ, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .ਵਿੱਚ_formatted_string = tok->ftstring_ਡੂੰਘਾਈ != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ਨਿਰਾ && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    ਜੇ (tok->level > 0) {
        ਪੂਰਨ level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ਵਾਪਸ info;
}

ਅਚਲ ਅੱਖਰ *
Py_Token_ਪਾਠ_View(ਅਚਲ ਬਣਤਰ tok_state *tok, ਅਚਲ ਬਣਤਰ token *token,
                  Py_ssize_t *ਲੰਮਾਈ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੰਮਾਈ != ਨਿਰਾ);
    ਜੇ (token->ਵਿਸਤਾਰ.ਸ਼ੁਰੂ < 0) {
        ਪ੍ਰਤਿਜ੍ਞਾ(token->ਵਿਸਤਾਰ.ਸ਼ੁਰੂ == -1 && token->ਵਿਸਤਾਰ.ਸਿਰਾ == -1);
        *ਲੰਮਾਈ = 0;
        ਵਾਪਸ "";
    }
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(&tok->ਸਰੋਤ, token->ਵਿਸਤਾਰ, ਲੰਮਾਈ);
}

ਅਚਲ ਅੱਖਰ *
Py_Tokenizer_ਵਿਸਤਾਰ_View(ਅਚਲ ਬਣਤਰ tok_state *tok, Py_Tok_ਵਿਸਤਾਰ ਵਿਸਤਾਰ,
                      Py_ssize_t *ਲੰਮਾਈ)
{
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਵਿਸਤਾਰ_View(&tok->ਸਰੋਤ, ਵਿਸਤਾਰ, ਲੰਮਾਈ);
}

ਖਾਲੀ
_PyToken_GetView(ਅਚਲ ਬਣਤਰ tok_state *tok, ਅਚਲ ਬਣਤਰ token *token,
                 _PyToken_View *view)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(view != ਨਿਰਾ);
    view->ਪਾਠ = Py_Token_ਪਾਠ_View(tok, token, &view->ਲੰਮਾਈ);
    view->ਸਿਰਾ_ਸਤਰ_ਸ਼ੁਰੂ = tok->ਸਤਰ_ਸ਼ੁਰੂ;
    view->ਸਤਰ_ਵਿਸਤਾਰ = (Py_Tok_ਵਿਸਤਾਰ){
        ISSTRINGLIT(token->type)
            ? token->ਵਿਸਤਾਰ.ਸ਼ੁਰੂ - token->ਸ਼ੁਰੂ_loc.byte_ਖਾਨਾ : tok->ਸਤਰ_ਸ਼ੁਰੂ,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->ਪੂਰਾ == E_EOF;
}

ਅਚਲ ਅੱਖਰ *
Py_Tokenizer_ਸਤਰ_View(ਅਚਲ ਬਣਤਰ tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *ਲੰਮਾਈ)
{
    ਵਾਪਸ Py_Tok_ਸਰੋਤ_ਸਤਰ_View(&tok->ਸਰੋਤ, lineno, ਲੰਮਾਈ);
}

ਖਾਲੀ
Py_Tokenizer_ਸੈੱਟ_Context(ਬਣਤਰ tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

ਖਾਲੀ
Py_Tokenizer_ਸੈੱਟ_Options(ਬਣਤਰ tok_state *tok, ਪੂਰਨ extra_ਟੋਕਨ,
                        ਪੂਰਨ type_comments)
{
    tok->tok_extra_ਟੋਕਨ = extra_ਟੋਕਨ;
    tok->type_comments = type_comments;
}

ਖਾਲੀ
_PyTokenizer_ImplyDedents(ਬਣਤਰ tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

ਖਾਲੀ
Py_Tokenizer_Allow_Incomplete_ਨਿਵੇਸ਼(ਬਣਤਰ tok_state *tok, ਪੂਰਨ allow)
{
    tok->allow_incomplete_ਨਿਵੇਸ਼ = allow != 0;
}

ਪੂਰਨ
_PyTokenizer_HasTrailingStatement(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off ਸਿਰਾ = tok->ਸਰੋਤ.ਅਧਾਰ_ਵਿੱਥ + tok->ਸਰੋਤ.len;
    ਜਦੋਂ (cur < ਸਿਰਾ) {
        ਪੂਰਨ c = Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, cur++);
        ਜੇ (c == '\0') {
            ਵਾਪਸ 0;
        }
        ਜੇ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ਜਾਰੀ;
        }
        ਜੇ (c != '#') {
            ਵਾਪਸ 1;
        }
        ਜਦੋਂ (cur < ਸਿਰਾ) {
            c = Py_Tok_ਸਰੋਤ_Byte(&tok->ਸਰੋਤ, cur);
            ਜੇ (c == '\0' || c == '\n') {
                ਰੋਕੋ;
            }
            cur++;
        }
    }
    ਵਾਪਸ 0;
}

ਪੂਰਨ
_PyTokenizer_IsInteractive(ਅਚਲ ਬਣਤਰ tok_state *tok)
{
    ਵਾਪਸ _PyTok_ReaderIsInteractive(tok);
}

ਖਾਲੀ
_PyTokenizer_StopInteractive(ਬਣਤਰ tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
