#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_token.h"

#شامل "tokenizer.h"
#شامل "reader.h"
#شامل "reader_internal.h"
#شامل "../lexer/state.h"

صحیح
Py_Tokenizer_Get_حالت(ثابت ساخت tok_state *tok)
{
    واپس tok->مکمل;
}

صحیح
_PyTokenizer_GetNewlineRequirements(ثابت ساخت tok_state *tok)
{
    واپس tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ثابت ساخت tok_state *tok)
{
    _PyTokenizer_Info info = {
        .حالت_2 = tok->مکمل,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سطر_آغاز < 0
            ? -1 : (صحیح)(tok->cur - tok->سطر_آغاز)},
        .cursor = tok->cur,
        .داخلہ_وسعت = {tok->buf_انحراف, tok->inp},
        .سطر_وسعت = {tok->سطر_آغاز, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .اندر_formatted_string = tok->ftstring_گہرائی != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != تہی && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    اگر (tok->level > 0) {
        صحیح level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    واپس info;
}

ثابت حرف *
Py_Token_متن_View(ثابت ساخت tok_state *tok, ثابت ساخت token *token,
                  Py_ssize_t *لمبائی_2)
{
    أكد(لمبائی_2 != تہی);
    اگر (token->وسعت.آغاز < 0) {
        أكد(token->وسعت.آغاز == -1 && token->وسعت.انتہا == -1);
        *لمبائی_2 = 0;
        واپس "";
    }
    واپس Py_Tok_ماخذ_وسعت_View(&tok->ماخذ, token->وسعت, لمبائی_2);
}

ثابت حرف *
Py_Tokenizer_وسعت_View(ثابت ساخت tok_state *tok, Py_Tok_وسعت وسعت,
                      Py_ssize_t *لمبائی_2)
{
    واپس Py_Tok_ماخذ_وسعت_View(&tok->ماخذ, وسعت, لمبائی_2);
}

خالی
_PyToken_GetView(ثابت ساخت tok_state *tok, ثابت ساخت token *token,
                 _PyToken_View *view)
{
    أكد(view != تہی);
    view->متن = Py_Token_متن_View(tok, token, &view->لمبائی_2);
    view->انتہا_سطر_آغاز = tok->سطر_آغاز;
    view->سطر_وسعت = (Py_Tok_وسعت){
        ISSTRINGLIT(token->type)
            ? token->وسعت.آغاز - token->آغاز_loc.byte_ستون : tok->سطر_آغاز,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->مکمل == E_EOF;
}

ثابت حرف *
Py_Tokenizer_سطر_View(ثابت ساخت tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *لمبائی_2)
{
    واپس Py_Tok_ماخذ_سطر_View(&tok->ماخذ, lineno, لمبائی_2);
}

خالی
Py_Tokenizer_سیٹ_Context(ساخت tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

خالی
Py_Tokenizer_سیٹ_Options(ساخت tok_state *tok, صحیح extra_علامات,
                        صحیح type_comments)
{
    tok->tok_extra_علامات = extra_علامات;
    tok->type_comments = type_comments;
}

خالی
_PyTokenizer_ImplyDedents(ساخت tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

خالی
Py_Tokenizer_Allow_Incomplete_داخلہ(ساخت tok_state *tok, صحیح allow)
{
    tok->allow_incomplete_داخلہ = allow != 0;
}

صحیح
_PyTokenizer_HasTrailingStatement(ثابت ساخت tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off انتہا = tok->ماخذ.بنیاد_انحراف + tok->ماخذ.len;
    جب (cur < انتہا) {
        صحیح c = Py_Tok_ماخذ_Byte(&tok->ماخذ, cur++);
        اگر (c == '\0') {
            واپس 0;
        }
        اگر (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            جاری;
        }
        اگر (c != '#') {
            واپس 1;
        }
        جب (cur < انتہا) {
            c = Py_Tok_ماخذ_Byte(&tok->ماخذ, cur);
            اگر (c == '\0' || c == '\n') {
                توڑو;
            }
            cur++;
        }
    }
    واپس 0;
}

صحیح
_PyTokenizer_IsInteractive(ثابت ساخت tok_state *tok)
{
    واپس _PyTok_ReaderIsInteractive(tok);
}

خالی
_PyTokenizer_StopInteractive(ساخت tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
