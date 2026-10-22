#شامل "Python.h"
#شامل "errcode.h"
#شامل "pycore_token.h"

#شامل "tokenizer.h"
#شامل "reader.h"
#شامل "reader_internal.h"
#شامل "../lexer/state.h"

صحیح
_PyTokenizer_GetStatus(ثابت ساختار tok_state *tok)
{
    بازگشت tok->done;
}

صحیح
_PyTokenizer_GetNewlineRequirements(ثابت ساختار tok_state *tok)
{
    بازگشت tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ثابت ساختار tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سطر_start < 0
            ? -1 : (صحیح)(tok->cur - tok->سطر_start)},
        .cursor = tok->cur,
        .درون_داد_span = {tok->buf_offset, tok->inp},
        .سطر_span = {tok->سطر_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != پوچ && tok->reader->fp != stdin,
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
    بازگشت info;
}

ثابت نویسه *
_PyToken_TextView(ثابت ساختار tok_state *tok, ثابت ساختار token *token,
                  Py_ssize_t *length)
{
    أكد(length != پوچ);
    اگر (token->span.start < 0) {
        أكد(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        بازگشت "";
    }
    بازگشت _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ثابت نویسه *
_PyTokenizer_SpanView(ثابت ساختار tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    بازگشت _PyTok_SourceSpanView(&tok->source, span, length);
}

تهی
_PyToken_GetView(ثابت ساختار tok_state *tok, ثابت ساختار token *token,
                 _PyToken_View *view)
{
    أكد(view != پوچ);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_سطر_start = tok->سطر_start;
    view->سطر_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->سطر_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ثابت نویسه *
Py_Tokenizer_سطر_View(ثابت ساختار tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    بازگشت Py_Tok_Source_سطر_View(&tok->source, lineno, length);
}

تهی
_PyTokenizer_SetContext(ساختار tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

تهی
_PyTokenizer_SetOptions(ساختار tok_state *tok, صحیح extra_tokens,
                        صحیح type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

تهی
_PyTokenizer_ImplyDedents(ساختار tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

تهی
Py_Tokenizer_Allow_Incomplete_درون_داد(ساختار tok_state *tok, صحیح allow)
{
    tok->allow_incomplete_درون_داد = allow != 0;
}

صحیح
_PyTokenizer_HasTrailingStatement(ثابت ساختار tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    وقتی (cur < end) {
        صحیح c = _PyTok_SourceByte(&tok->source, cur++);
        اگر (c == '\0') {
            بازگشت 0;
        }
        اگر (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ادامه;
        }
        اگر (c != '#') {
            بازگشت 1;
        }
        وقتی (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            اگر (c == '\0' || c == '\n') {
                بشکن;
            }
            cur++;
        }
    }
    بازگشت 0;
}

صحیح
_PyTokenizer_IsInteractive(ثابت ساختار tok_state *tok)
{
    بازگشت _PyTok_ReaderIsInteractive(tok);
}

تهی
_PyTokenizer_StopInteractive(ساختار tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
