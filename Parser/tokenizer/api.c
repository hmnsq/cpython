#অন্তর্ভাৱ "Python.h"
#অন্তর্ভাৱ "errcode.h"
#অন্তর্ভাৱ "pycore_token.h"

#অন্তর্ভাৱ "tokenizer.h"
#অন্তর্ভাৱ "reader.h"
#অন্তর্ভাৱ "reader_internal.h"
#অন্তর্ভাৱ "../lexer/state.h"

পূর্ণাঙ্ক
_PyTokenizer_GetStatus(ধ্রুৱ সংরচনা tok_state *tok)
{
    ঘূৰাই tok->done;
}

পূর্ণাঙ্ক
_PyTokenizer_GetNewlineRequirements(ধ্রুৱ সংরচনা tok_state *tok)
{
    ঘূৰাই tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ধ্রুৱ সংরচনা tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->সাৰি_start < 0
            ? -1 : (পূর্ণাঙ্ক)(tok->cur - tok->সাৰি_start)},
        .cursor = tok->cur,
        .প্ৰবেশ_span = {tok->buf_offset, tok->inp},
        .সাৰি_span = {tok->সাৰি_start, tok->inp},
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
    ঘূৰাই info;
}

ধ্রুৱ ৱর্ণ *
_PyToken_TextView(ধ্রুৱ সংরচনা tok_state *tok, ধ্রুৱ সংরচনা token *token,
                  Py_ssize_t *length)
{
    প্রতিজ্ঞা(length != রিক্ত);
    যদি (token->span.start < 0) {
        প্রতিজ্ঞা(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        ঘূৰাই "";
    }
    ঘূৰাই _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ধ্রুৱ ৱর্ণ *
_PyTokenizer_SpanView(ধ্রুৱ সংরচনা tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ঘূৰাই _PyTok_SourceSpanView(&tok->source, span, length);
}

শূন্য
_PyToken_GetView(ধ্রুৱ সংরচনা tok_state *tok, ধ্রুৱ সংরচনা token *token,
                 _PyToken_View *view)
{
    প্রতিজ্ঞা(view != রিক্ত);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_সাৰি_start = tok->সাৰি_start;
    view->সাৰি_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->সাৰি_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ধ্রুৱ ৱর্ণ *
Py_Tokenizer_সাৰি_View(ধ্রুৱ সংরচনা tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    ঘূৰাই Py_Tok_Source_সাৰি_View(&tok->source, lineno, length);
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
Py_Tokenizer_Allow_Incomplete_প্ৰবেশ(সংরচনা tok_state *tok, পূর্ণাঙ্ক allow)
{
    tok->allow_incomplete_প্ৰবেশ = allow != 0;
}

পূর্ণাঙ্ক
_PyTokenizer_HasTrailingStatement(ধ্রুৱ সংরচনা tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    যেতিয়ালৈ (cur < end) {
        পূর্ণাঙ্ক c = _PyTok_SourceByte(&tok->source, cur++);
        যদি (c == '\0') {
            ঘূৰাই 0;
        }
        যদি (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            অনুৱর্ত;
        }
        যদি (c != '#') {
            ঘূৰাই 1;
        }
        যেতিয়ালৈ (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            যদি (c == '\0' || c == '\n') {
                ভঙ্গ;
            }
            cur++;
        }
    }
    ঘূৰাই 0;
}

পূর্ণাঙ্ক
_PyTokenizer_IsInteractive(ধ্রুৱ সংরচনা tok_state *tok)
{
    ঘূৰাই _PyTok_ReaderIsInteractive(tok);
}

শূন্য
_PyTokenizer_StopInteractive(সংরচনা tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
