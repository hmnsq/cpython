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
    أرجع tok->پورا;
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
        .حالت = tok->پورا,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سطر_شروع < 0
            ? -1 : (صحيح)(tok->cur - tok->سطر_شروع)},
        .cursor = tok->cur,
        .داخلہ_وسعت = {tok->buf_وتھ, tok->inp},
        .سطر_وسعت = {tok->سطر_شروع, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .وچ_formatted_string = tok->ftstring_ڈونگھائی != 0,
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
                  Py_ssize_t *لمائی)
{
    أكد(لمائی != عدم);
    إذا (token->وسعت.شروع < 0) {
        أكد(token->وسعت.شروع == -1 && token->وسعت.سرا == -1);
        *لمائی = 0;
        أرجع "";
    }
    أرجع Py_Tok_سروت_وسعت_View(&tok->سروت, token->وسعت, لمائی);
}

ثابت حرف *
Py_Tokenizer_وسعت_View(ثابت بنية tok_state *tok, Py_Tok_وسعت وسعت,
                      Py_ssize_t *لمائی)
{
    أرجع Py_Tok_سروت_وسعت_View(&tok->سروت, وسعت, لمائی);
}

فراغ
_PyToken_GetView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                 _PyToken_View *view)
{
    أكد(view != عدم);
    view->متن = Py_Token_متن_View(tok, token, &view->لمائی);
    view->سرا_سطر_شروع = tok->سطر_شروع;
    view->سطر_وسعت = (Py_Tok_وسعت){
        ISSTRINGLIT(token->type)
            ? token->وسعت.شروع - token->شروع_loc.byte_خانہ : tok->سطر_شروع,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->پورا == E_EOF;
}

ثابت حرف *
Py_Tokenizer_سطر_View(ثابت بنية tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *لمائی)
{
    أرجع Py_Tok_سروت_سطر_View(&tok->سروت, lineno, لمائی);
}

فراغ
Py_Tokenizer_سیٹ_Context(بنية tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

فراغ
Py_Tokenizer_سیٹ_Options(بنية tok_state *tok, صحيح extra_ٹوکن,
                        صحيح type_comments)
{
    tok->tok_extra_ٹوکن = extra_ٹوکن;
    tok->type_comments = type_comments;
}

فراغ
_PyTokenizer_ImplyDedents(بنية tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

فراغ
Py_Tokenizer_Allow_Incomplete_داخلہ(بنية tok_state *tok, صحيح allow)
{
    tok->allow_incomplete_داخلہ = allow != 0;
}

صحيح
_PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off سرا = tok->سروت.ادھار_وتھ + tok->سروت.len;
    طالما (cur < سرا) {
        صحيح c = Py_Tok_سروت_Byte(&tok->سروت, cur++);
        إذا (c == '\0') {
            أرجع 0;
        }
        إذا (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            استمر;
        }
        إذا (c != '#') {
            أرجع 1;
        }
        طالما (cur < سرا) {
            c = Py_Tok_سروت_Byte(&tok->سروت, cur);
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
