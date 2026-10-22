#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"

#ଅନ୍ତର୍ଭାଵ "tokenizer.h"
#ଅନ୍ତର୍ଭାଵ "reader.h"
#ଅନ୍ତର୍ଭାଵ "reader_internal.h"
#ଅନ୍ତର୍ଭାଵ "../lexer/state.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_GetStatus(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଫେରନ୍ତୁ tok->done;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_GetNewlineRequirements(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଫେରନ୍ତୁ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->ଧାଡ଼ି_start < 0
            ? -1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tok->cur - tok->ଧାଡ଼ି_start)},
        .cursor = tok->cur,
        .ନିବେଶ_span = {tok->buf_offset, tok->inp},
        .ଧାଡ଼ି_span = {tok->ଧାଡ଼ି_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ଖାଲି && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    ଯଦି (tok->level > 0) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ଫେରନ୍ତୁ info;
}

ଅଚଳ ଅକ୍ଷର *
_PyToken_TextView(ଅଚଳ ସଂରଚନା tok_state *tok, ଅଚଳ ସଂରଚନା token *token,
                  Py_ssize_t *length)
{
    ପ୍ରତିଜ୍ଞା(length != ଖାଲି);
    ଯଦି (token->span.start < 0) {
        ପ୍ରତିଜ୍ଞା(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        ଫେରନ୍ତୁ "";
    }
    ଫେରନ୍ତୁ _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ଅଚଳ ଅକ୍ଷର *
_PyTokenizer_SpanView(ଅଚଳ ସଂରଚନା tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ଫେରନ୍ତୁ _PyTok_SourceSpanView(&tok->source, span, length);
}

ଶୂନ୍ୟ
_PyToken_GetView(ଅଚଳ ସଂରଚନା tok_state *tok, ଅଚଳ ସଂରଚନା token *token,
                 _PyToken_View *view)
{
    ପ୍ରତିଜ୍ଞା(view != ଖାଲି);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_ଧାଡ଼ି_start = tok->ଧାଡ଼ି_start;
    view->ଧାଡ଼ି_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->ଧାଡ଼ି_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ଅଚଳ ଅକ୍ଷର *
Py_Tokenizer_ଧାଡ଼ି_View(ଅଚଳ ସଂରଚନା tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    ଫେରନ୍ତୁ Py_Tok_Source_ଧାଡ଼ି_View(&tok->source, lineno, length);
}

ଶୂନ୍ୟ
_PyTokenizer_SetContext(ସଂରଚନା tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

ଶୂନ୍ୟ
_PyTokenizer_SetOptions(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_tokens,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

ଶୂନ୍ୟ
_PyTokenizer_ImplyDedents(ସଂରଚନା tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

ଶୂନ୍ୟ
Py_Tokenizer_Allow_Incomplete_ନିବେଶ(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow)
{
    tok->allow_incomplete_ନିବେଶ = allow != 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_HasTrailingStatement(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    ଯେତେବେଳେ (cur < end) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = _PyTok_SourceByte(&tok->source, cur++);
        ଯଦି (c == '\0') {
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ଜାରି;
        }
        ଯଦି (c != '#') {
            ଫେରନ୍ତୁ 1;
        }
        ଯେତେବେଳେ (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            ଯଦି (c == '\0' || c == '\n') {
                ଭାଙ୍ଗ;
            }
            cur++;
        }
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyTokenizer_IsInteractive(ଅଚଳ ସଂରଚନା tok_state *tok)
{
    ଫେରନ୍ତୁ _PyTok_ReaderIsInteractive(tok);
}

ଶୂନ୍ୟ
_PyTokenizer_StopInteractive(ସଂରଚନା tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
