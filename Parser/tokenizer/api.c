#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "tokenizer.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

صحيح
Py_Tokenizer_Get_حالت(ثابت بنية tok_state *tok)
{
    أرجع tok->بشپړ;
}

صحيح
_PyTokenizer_GetNewlineRequirements(ثابت بنية tok_state *tok)
{
    أرجع tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ثابت بنية tok_state *tok)
{
    _PyTokenizer_Info info = {
        .حالت = tok->بشپړ,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->کرښه_پیل < 0
            ? -1 : (صحيح)(tok->cur - tok->کرښه_پیل)},
        .cursor = tok->cur,
        .ننوت_پراختیا = {tok->buf_دننه_واټن, tok->inp},
        .کرښه_پراختیا = {tok->کرښه_پیل, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .دننه_formatted_string = tok->ftstring_ژوروالی != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != عدم && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    إذا (tok->level > 0) {
        صحيح level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    أرجع info;
}

ثابت حرف *
Py_Token_متن_View(ثابت بنية tok_state *tok, ثابت بنية token *token,
                  Py_ssize_t *اوږدتیا)
{
    أكد(اوږدتیا != عدم);
    إذا (token->پراختیا.پیل < 0) {
        أكد(token->پراختیا.پیل == -1 && token->پراختیا.پای_ټکی == -1);
        *اوږدتیا = 0;
        أرجع "";
    }
    أرجع Py_Tok_سرچینه_پراختیا_View(&tok->سرچینه, token->پراختیا, اوږدتیا);
}

ثابت حرف *
Py_Tokenizer_پراختیا_View(ثابت بنية tok_state *tok, Py_Tok_پراختیا پراختیا,
                      Py_ssize_t *اوږدتیا)
{
    أرجع Py_Tok_سرچینه_پراختیا_View(&tok->سرچینه, پراختیا, اوږدتیا);
}

فراغ
_PyToken_GetView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                 _PyToken_View *view)
{
    أكد(view != عدم);
    view->متن = Py_Token_متن_View(tok, token, &view->اوږدتیا);
    view->پای_ټکی_کرښه_پیل = tok->کرښه_پیل;
    view->کرښه_پراختیا = (Py_Tok_پراختیا){
        ISSTRINGLIT(token->type)
            ? token->پراختیا.پیل - token->پیل_loc.byte_ستون : tok->کرښه_پیل,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->بشپړ == E_EOF;
}

ثابت حرف *
Py_Tokenizer_کرښه_View(ثابت بنية tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *اوږدتیا)
{
    أرجع Py_Tok_سرچینه_کرښه_View(&tok->سرچینه, lineno, اوږدتیا);
}

فراغ
Py_Tokenizer_سېټ_Context(بنية tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

فراغ
Py_Tokenizer_سېټ_Options(بنية tok_state *tok, صحيح extra_نښې,
                        صحيح type_comments)
{
    tok->tok_extra_نښې = extra_نښې;
    tok->type_comments = type_comments;
}

فراغ
_PyTokenizer_ImplyDedents(بنية tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

فراغ
Py_Tokenizer_Allow_Incomplete_ننوت(بنية tok_state *tok, صحيح allow)
{
    tok->allow_incomplete_ننوت = allow != 0;
}

صحيح
_PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off پای_ټکی = tok->سرچینه.بنسټ_دننه_واټن + tok->سرچینه.len;
    طالما (cur < پای_ټکی) {
        صحيح c = Py_Tok_سرچینه_Byte(&tok->سرچینه, cur++);
        إذا (c == '\0') {
            أرجع 0;
        }
        إذا (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            استمر;
        }
        إذا (c != '#') {
            أرجع 1;
        }
        طالما (cur < پای_ټکی) {
            c = Py_Tok_سرچینه_Byte(&tok->سرچینه, cur);
            إذا (c == '\0' || c == '\n') {
                اكسر;
            }
            cur++;
        }
    }
    أرجع 0;
}

صحيح
_PyTokenizer_IsInteractive(ثابت بنية tok_state *tok)
{
    أرجع _PyTok_ReaderIsInteractive(tok);
}

فراغ
_PyTokenizer_StopInteractive(بنية tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
