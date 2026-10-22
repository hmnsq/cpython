#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "tokenizer.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

صحيح
Py_Tokenizer_Get_وضعیت(ثابت بنية tok_state *tok)
{
    أرجع tok->انجامشده;
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
        .وضعیت = tok->انجامشده,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سطر_شروع < 0
            ? -1 : (صحيح)(tok->cur - tok->سطر_شروع)},
        .cursor = tok->cur,
        .درون_داد_بازه = {tok->buf_جابه_جایی, tok->inp},
        .سطر_بازه = {tok->سطر_شروع, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .درون_formatted_string = tok->ftstring_عمق != 0,
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
                  Py_ssize_t *درازی)
{
    أكد(درازی != عدم);
    إذا (token->بازه.شروع < 0) {
        أكد(token->بازه.شروع == -1 && token->بازه.انتها == -1);
        *درازی = 0;
        أرجع "";
    }
    أرجع Py_Tok_منبع_بازه_View(&tok->منبع, token->بازه, درازی);
}

ثابت حرف *
Py_Tokenizer_بازه_View(ثابت بنية tok_state *tok, Py_Tok_بازه بازه,
                      Py_ssize_t *درازی)
{
    أرجع Py_Tok_منبع_بازه_View(&tok->منبع, بازه, درازی);
}

فراغ
_PyToken_GetView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                 _PyToken_View *view)
{
    أكد(view != عدم);
    view->متن = Py_Token_متن_View(tok, token, &view->درازی);
    view->انتها_سطر_شروع = tok->سطر_شروع;
    view->سطر_بازه = (Py_Tok_بازه){
        ISSTRINGLIT(token->type)
            ? token->بازه.شروع - token->شروع_loc.byte_ستون : tok->سطر_شروع,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->انجامشده == E_EOF;
}

ثابت حرف *
Py_Tokenizer_سطر_View(ثابت بنية tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *درازی)
{
    أرجع Py_Tok_منبع_سطر_View(&tok->منبع, lineno, درازی);
}

فراغ
Py_Tokenizer_مجموعه_Context(بنية tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

فراغ
Py_Tokenizer_مجموعه_Options(بنية tok_state *tok, صحيح extra_نشانهها,
                        صحيح type_comments)
{
    tok->tok_extra_نشانهها = extra_نشانهها;
    tok->type_comments = type_comments;
}

فراغ
_PyTokenizer_ImplyDedents(بنية tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

فراغ
Py_Tokenizer_Allow_Incomplete_درون_داد(بنية tok_state *tok, صحيح allow)
{
    tok->allow_incomplete_درون_داد = allow != 0;
}

صحيح
_PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off انتها = tok->منبع.پایه_جابه_جایی + tok->منبع.len;
    طالما (cur < انتها) {
        صحيح c = Py_Tok_منبع_Byte(&tok->منبع, cur++);
        إذا (c == '\0') {
            أرجع 0;
        }
        إذا (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            استمر;
        }
        إذا (c != '#') {
            أرجع 1;
        }
        طالما (cur < انتها) {
            c = Py_Tok_منبع_Byte(&tok->منبع, cur);
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
