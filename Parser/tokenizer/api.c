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
    أرجع tok->پورو;
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
        .حالت = tok->پورو,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سٽ_شروعات < 0
            ? -1 : (صحيح)(tok->cur - tok->سٽ_شروعات)},
        .cursor = tok->cur,
        .داخلا_وسعت = {tok->buf_وٿي, tok->inp},
        .سٽ_وسعت = {tok->سٽ_شروعات, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .اندريون_formatted_string = tok->ftstring_اونهائي != 0,
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
                  Py_ssize_t *ڊيگهائي)
{
    أكد(ڊيگهائي != عدم);
    إذا (token->وسعت.شروعات < 0) {
        أكد(token->وسعت.شروعات == -1 && token->وسعت.پڇاڙي == -1);
        *ڊيگهائي = 0;
        أرجع "";
    }
    أرجع Py_Tok_ذريعو_وسعت_View(&tok->ذريعو, token->وسعت, ڊيگهائي);
}

ثابت حرف *
Py_Tokenizer_وسعت_View(ثابت بنية tok_state *tok, Py_Tok_وسعت وسعت,
                      Py_ssize_t *ڊيگهائي)
{
    أرجع Py_Tok_ذريعو_وسعت_View(&tok->ذريعو, وسعت, ڊيگهائي);
}

فراغ
_PyToken_GetView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                 _PyToken_View *view)
{
    أكد(view != عدم);
    view->متن = Py_Token_متن_View(tok, token, &view->ڊيگهائي);
    view->پڇاڙي_سٽ_شروعات = tok->سٽ_شروعات;
    view->سٽ_وسعت = (Py_Tok_وسعت){
        ISSTRINGLIT(token->type)
            ? token->وسعت.شروعات - token->شروعات_loc.byte_کانو : tok->سٽ_شروعات,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->پورو == E_EOF;
}

ثابت حرف *
Py_Tokenizer_سٽ_View(ثابت بنية tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *ڊيگهائي)
{
    أرجع Py_Tok_ذريعو_سٽ_View(&tok->ذريعو, lineno, ڊيگهائي);
}

فراغ
Py_Tokenizer_سيٽ_Context(بنية tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

فراغ
Py_Tokenizer_سيٽ_Options(بنية tok_state *tok, صحيح extra_ٽوڪن,
                        صحيح type_comments)
{
    tok->tok_extra_ٽوڪن = extra_ٽوڪن;
    tok->type_comments = type_comments;
}

فراغ
_PyTokenizer_ImplyDedents(بنية tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

فراغ
Py_Tokenizer_Allow_Incomplete_داخلا(بنية tok_state *tok, صحيح allow)
{
    tok->allow_incomplete_داخلا = allow != 0;
}

صحيح
_PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off پڇاڙي = tok->ذريعو.بنياد_وٿي + tok->ذريعو.len;
    طالما (cur < پڇاڙي) {
        صحيح c = Py_Tok_ذريعو_Byte(&tok->ذريعو, cur++);
        إذا (c == '\0') {
            أرجع 0;
        }
        إذا (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            استمر;
        }
        إذا (c != '#') {
            أرجع 1;
        }
        طالما (cur < پڇاڙي) {
            c = Py_Tok_ذريعو_Byte(&tok->ذريعو, cur);
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
