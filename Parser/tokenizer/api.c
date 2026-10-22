#અન્તર્ભાવ "Python.h"
#અન્તર્ભાવ "errcode.h"
#અન્તર્ભાવ "pycore_token.h"

#અન્તર્ભાવ "tokenizer.h"
#અન્તર્ભાવ "reader.h"
#અન્તર્ભાવ "reader_internal.h"
#અન્તર્ભાવ "../lexer/state.h"

પૂર્ણાંક
_PyTokenizer_GetStatus(અચલ સંરચના tok_state *tok)
{
    પાછા tok->done;
}

પૂર્ણાંક
_PyTokenizer_GetNewlineRequirements(અચલ સંરચના tok_state *tok)
{
    પાછા tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(અચલ સંરચના tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->લીટી_start < 0
            ? -1 : (પૂર્ણાંક)(tok->cur - tok->લીટી_start)},
        .cursor = tok->cur,
        .આગત_span = {tok->buf_offset, tok->inp},
        .લીટી_span = {tok->લીટી_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ખાલી && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    જો (tok->level > 0) {
        પૂર્ણાંક level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    પાછા info;
}

અચલ અક્ષર *
_PyToken_TextView(અચલ સંરચના tok_state *tok, અચલ સંરચના token *token,
                  Py_ssize_t *length)
{
    પ્રતિજ્ઞા(length != ખાલી);
    જો (token->span.start < 0) {
        પ્રતિજ્ઞા(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        પાછા "";
    }
    પાછા _PyTok_SourceSpanView(&tok->source, token->span, length);
}

અચલ અક્ષર *
_PyTokenizer_SpanView(અચલ સંરચના tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    પાછા _PyTok_SourceSpanView(&tok->source, span, length);
}

શૂન્ય
_PyToken_GetView(અચલ સંરચના tok_state *tok, અચલ સંરચના token *token,
                 _PyToken_View *view)
{
    પ્રતિજ્ઞા(view != ખાલી);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_લીટી_start = tok->લીટી_start;
    view->લીટી_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->લીટી_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

અચલ અક્ષર *
Py_Tokenizer_લીટી_View(અચલ સંરચના tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    પાછા Py_Tok_Source_લીટી_View(&tok->source, lineno, length);
}

શૂન્ય
_PyTokenizer_SetContext(સંરચના tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

શૂન્ય
_PyTokenizer_SetOptions(સંરચના tok_state *tok, પૂર્ણાંક extra_tokens,
                        પૂર્ણાંક type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

શૂન્ય
_PyTokenizer_ImplyDedents(સંરચના tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

શૂન્ય
Py_Tokenizer_Allow_Incomplete_આગત(સંરચના tok_state *tok, પૂર્ણાંક allow)
{
    tok->allow_incomplete_આગત = allow != 0;
}

પૂર્ણાંક
_PyTokenizer_HasTrailingStatement(અચલ સંરચના tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    જ્યારે (cur < end) {
        પૂર્ણાંક c = _PyTok_SourceByte(&tok->source, cur++);
        જો (c == '\0') {
            પાછા 0;
        }
        જો (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ચાલુ;
        }
        જો (c != '#') {
            પાછા 1;
        }
        જ્યારે (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            જો (c == '\0' || c == '\n') {
                તોડો;
            }
            cur++;
        }
    }
    પાછા 0;
}

પૂર્ણાંક
_PyTokenizer_IsInteractive(અચલ સંરચના tok_state *tok)
{
    પાછા _PyTok_ReaderIsInteractive(tok);
}

શૂન્ય
_PyTokenizer_StopInteractive(સંરચના tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
