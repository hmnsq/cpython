#அந்தர்பாவ "Python.h"
#அந்தர்பாவ "errcode.h"
#அந்தர்பாவ "pycore_token.h"

#அந்தர்பாவ "tokenizer.h"
#அந்தர்பாவ "reader.h"
#அந்தர்பாவ "reader_internal.h"
#அந்தர்பாவ "../lexer/state.h"

முழு
_PyTokenizer_GetStatus(மாறிலி கட்டமைப்பு tok_state *tok)
{
    திரும்பு tok->done;
}

முழு
_PyTokenizer_GetNewlineRequirements(மாறிலி கட்டமைப்பு tok_state *tok)
{
    திரும்பு tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(மாறிலி கட்டமைப்பு tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->வரி_start < 0
            ? -1 : (முழு)(tok->cur - tok->வரி_start)},
        .cursor = tok->cur,
        .உள்ளீடு_span = {tok->buf_offset, tok->inp},
        .வரி_span = {tok->வரி_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != வெற்று && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    எனில் (tok->level > 0) {
        முழு level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    திரும்பு info;
}

மாறிலி எழுத்து *
_PyToken_TextView(மாறிலி கட்டமைப்பு tok_state *tok, மாறிலி கட்டமைப்பு token *token,
                  Py_ssize_t *length)
{
    ப்ரதிஜ்ஞா(length != வெற்று);
    எனில் (token->span.start < 0) {
        ப்ரதிஜ்ஞா(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        திரும்பு "";
    }
    திரும்பு _PyTok_SourceSpanView(&tok->source, token->span, length);
}

மாறிலி எழுத்து *
_PyTokenizer_SpanView(மாறிலி கட்டமைப்பு tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    திரும்பு _PyTok_SourceSpanView(&tok->source, span, length);
}

வெறுமை
_PyToken_GetView(மாறிலி கட்டமைப்பு tok_state *tok, மாறிலி கட்டமைப்பு token *token,
                 _PyToken_View *view)
{
    ப்ரதிஜ்ஞா(view != வெற்று);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_வரி_start = tok->வரி_start;
    view->வரி_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->வரி_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

மாறிலி எழுத்து *
Py_Tokenizer_வரி_View(மாறிலி கட்டமைப்பு tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    திரும்பு Py_Tok_Source_வரி_View(&tok->source, lineno, length);
}

வெறுமை
_PyTokenizer_SetContext(கட்டமைப்பு tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

வெறுமை
_PyTokenizer_SetOptions(கட்டமைப்பு tok_state *tok, முழு extra_tokens,
                        முழு type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

வெறுமை
_PyTokenizer_ImplyDedents(கட்டமைப்பு tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

வெறுமை
Py_Tokenizer_Allow_Incomplete_உள்ளீடு(கட்டமைப்பு tok_state *tok, முழு allow)
{
    tok->allow_incomplete_உள்ளீடு = allow != 0;
}

முழு
_PyTokenizer_HasTrailingStatement(மாறிலி கட்டமைப்பு tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    போது (cur < end) {
        முழு c = _PyTok_SourceByte(&tok->source, cur++);
        எனில் (c == '\0') {
            திரும்பு 0;
        }
        எனில் (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            தொடர்;
        }
        எனில் (c != '#') {
            திரும்பு 1;
        }
        போது (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            எனில் (c == '\0' || c == '\n') {
                நிறுத்து;
            }
            cur++;
        }
    }
    திரும்பு 0;
}

முழு
_PyTokenizer_IsInteractive(மாறிலி கட்டமைப்பு tok_state *tok)
{
    திரும்பு _PyTok_ReaderIsInteractive(tok);
}

வெறுமை
_PyTokenizer_StopInteractive(கட்டமைப்பு tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
