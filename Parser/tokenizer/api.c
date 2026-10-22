#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "tokenizer.h"
#ضمن "reader.h"
#ضمن "reader_internal.h"
#ضمن "../lexer/state.h"

صحيح
_PyTokenizer_GetStatus(ثابت بنية tok_state *tok)
{
    أرجع tok->done;
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
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->سطر_start < 0
            ? -1 : (صحيح)(tok->cur - tok->سطر_start)},
        .cursor = tok->cur,
        .مدخل_span = {tok->buf_offset, tok->inp},
        .سطر_span = {tok->سطر_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
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
_PyToken_TextView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                  Py_ssize_t *length)
{
    أكد(length != عدم);
    إذا (token->span.start < 0) {
        أكد(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        أرجع "";
    }
    أرجع _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ثابت حرف *
_PyTokenizer_SpanView(ثابت بنية tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    أرجع _PyTok_SourceSpanView(&tok->source, span, length);
}

فراغ
_PyToken_GetView(ثابت بنية tok_state *tok, ثابت بنية token *token,
                 _PyToken_View *view)
{
    أكد(view != عدم);
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

ثابت حرف *
Py_Tokenizer_سطر_View(ثابت بنية tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    أرجع Py_Tok_Source_سطر_View(&tok->source, lineno, length);
}

فراغ
_PyTokenizer_SetContext(بنية tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

فراغ
_PyTokenizer_SetOptions(بنية tok_state *tok, صحيح extra_tokens,
                        صحيح type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

فراغ
_PyTokenizer_ImplyDedents(بنية tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

فراغ
Py_Tokenizer_Allow_Incomplete_مدخل(بنية tok_state *tok, صحيح allow)
{
    tok->allow_incomplete_مدخل = allow != 0;
}

صحيح
_PyTokenizer_HasTrailingStatement(ثابت بنية tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    طالما (cur < end) {
        صحيح c = _PyTok_SourceByte(&tok->source, cur++);
        إذا (c == '\0') {
            أرجع 0;
        }
        إذا (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            استمر;
        }
        إذا (c != '#') {
            أرجع 1;
        }
        طالما (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
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
