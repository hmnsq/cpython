#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "pycore_token.h"

#অন্তর্ভাব "tokenizer.h"
#অন্তর্ভাব "reader.h"
#অন্তর্ভাব "reader_internal.h"
#অন্তর্ভাব "../lexer/state.h"

পূর্ণাঙ্ক
_PyTokenizer_GetStatus(ধ্রুব সংরচনা tok_state *tok)
{
    প্রত্যাগম tok->done;
}

পূর্ণাঙ্ক
_PyTokenizer_GetNewlineRequirements(ধ্রুব সংরচনা tok_state *tok)
{
    প্রত্যাগম tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ধ্রুব সংরচনা tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->সারি_start < 0
            ? -1 : (পূর্ণাঙ্ক)(tok->cur - tok->সারি_start)},
        .cursor = tok->cur,
        .প্রবেশ_span = {tok->buf_offset, tok->inp},
        .সারি_span = {tok->সারি_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != রিক্ত && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    যদি (tok->level > 0) {
        পূর্ণাঙ্ক level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    প্রত্যাগম info;
}

ধ্রুব বর্ণ *
_PyToken_TextView(ধ্রুব সংরচনা tok_state *tok, ধ্রুব সংরচনা token *token,
                  Py_ssize_t *length)
{
    প্রতিজ্ঞা(length != রিক্ত);
    যদি (token->span.start < 0) {
        প্রতিজ্ঞা(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        প্রত্যাগম "";
    }
    প্রত্যাগম _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ধ্রুব বর্ণ *
_PyTokenizer_SpanView(ধ্রুব সংরচনা tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    প্রত্যাগম _PyTok_SourceSpanView(&tok->source, span, length);
}

শূন্য
_PyToken_GetView(ধ্রুব সংরচনা tok_state *tok, ধ্রুব সংরচনা token *token,
                 _PyToken_View *view)
{
    প্রতিজ্ঞা(view != রিক্ত);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_সারি_start = tok->সারি_start;
    view->সারি_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->সারি_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ধ্রুব বর্ণ *
Py_Tokenizer_সারি_View(ধ্রুব সংরচনা tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    প্রত্যাগম Py_Tok_Source_সারি_View(&tok->source, lineno, length);
}

শূন্য
_PyTokenizer_SetContext(সংরচনা tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

শূন্য
_PyTokenizer_SetOptions(সংরচনা tok_state *tok, পূর্ণাঙ্ক extra_tokens,
                        পূর্ণাঙ্ক type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

শূন্য
_PyTokenizer_ImplyDedents(সংরচনা tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

শূন্য
Py_Tokenizer_Allow_Incomplete_প্রবেশ(সংরচনা tok_state *tok, পূর্ণাঙ্ক allow)
{
    tok->allow_incomplete_প্রবেশ = allow != 0;
}

পূর্ণাঙ্ক
_PyTokenizer_HasTrailingStatement(ধ্রুব সংরচনা tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    যাবত্ (cur < end) {
        পূর্ণাঙ্ক c = _PyTok_SourceByte(&tok->source, cur++);
        যদি (c == '\0') {
            প্রত্যাগম 0;
        }
        যদি (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            অনুবর্ত;
        }
        যদি (c != '#') {
            প্রত্যাগম 1;
        }
        যাবত্ (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            যদি (c == '\0' || c == '\n') {
                ভঙ্গ;
            }
            cur++;
        }
    }
    প্রত্যাগম 0;
}

পূর্ণাঙ্ক
_PyTokenizer_IsInteractive(ধ্রুব সংরচনা tok_state *tok)
{
    প্রত্যাগম _PyTok_ReaderIsInteractive(tok);
}

শূন্য
_PyTokenizer_StopInteractive(সংরচনা tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
