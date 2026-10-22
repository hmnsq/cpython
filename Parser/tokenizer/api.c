#অন্তর্ভাব "Python.h"
#অন্তর্ভাব "errcode.h"
#অন্তর্ভাব "pycore_token.h"

#অন্তর্ভাব "tokenizer.h"
#অন্তর্ভাব "reader.h"
#অন্তর্ভাব "reader_internal.h"
#অন্তর্ভাব "../lexer/state.h"

পূর্ণসংখ্যা
_PyTokenizer_GetStatus(ধ্রুবক কাঠামো tok_state *tok)
{
    ফেরত tok->done;
}

পূর্ণসংখ্যা
_PyTokenizer_GetNewlineRequirements(ধ্রুবক কাঠামো tok_state *tok)
{
    ফেরত tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ধ্রুবক কাঠামো tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->সারি_start < 0
            ? -1 : (পূর্ণসংখ্যা)(tok->cur - tok->সারি_start)},
        .cursor = tok->cur,
        .প্রবেশ_span = {tok->buf_offset, tok->inp},
        .সারি_span = {tok->সারি_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ফাঁকা && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    যদি (tok->level > 0) {
        পূর্ণসংখ্যা level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ফেরত info;
}

ধ্রুবক অক্ষর *
_PyToken_TextView(ধ্রুবক কাঠামো tok_state *tok, ধ্রুবক কাঠামো token *token,
                  Py_ssize_t *দৈর্ঘ্য)
{
    প্রতিজ্ঞা(দৈর্ঘ্য != ফাঁকা);
    যদি (token->span.start < 0) {
        প্রতিজ্ঞা(token->span.start == -1 && token->span.end == -1);
        *দৈর্ঘ্য = 0;
        ফেরত "";
    }
    ফেরত _PyTok_SourceSpanView(&tok->source, token->span, দৈর্ঘ্য);
}

ধ্রুবক অক্ষর *
_PyTokenizer_SpanView(ধ্রুবক কাঠামো tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *দৈর্ঘ্য)
{
    ফেরত _PyTok_SourceSpanView(&tok->source, span, দৈর্ঘ্য);
}

শূন্য
_PyToken_GetView(ধ্রুবক কাঠামো tok_state *tok, ধ্রুবক কাঠামো token *token,
                 _PyToken_View *view)
{
    প্রতিজ্ঞা(view != ফাঁকা);
    view->text = _PyToken_TextView(tok, token, &view->দৈর্ঘ্য);
    view->end_সারি_start = tok->সারি_start;
    view->সারি_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->সারি_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ধ্রুবক অক্ষর *
Py_Tokenizer_সারি_View(ধ্রুবক কাঠামো tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *দৈর্ঘ্য)
{
    ফেরত Py_Tok_Source_সারি_View(&tok->source, lineno, দৈর্ঘ্য);
}

শূন্য
_PyTokenizer_SetContext(কাঠামো tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

শূন্য
_PyTokenizer_SetOptions(কাঠামো tok_state *tok, পূর্ণসংখ্যা extra_tokens,
                        পূর্ণসংখ্যা type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

শূন্য
_PyTokenizer_ImplyDedents(কাঠামো tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

শূন্য
Py_Tokenizer_Allow_Incomplete_প্রবেশ(কাঠামো tok_state *tok, পূর্ণসংখ্যা allow)
{
    tok->allow_incomplete_প্রবেশ = allow != 0;
}

পূর্ণসংখ্যা
_PyTokenizer_HasTrailingStatement(ধ্রুবক কাঠামো tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    যখন (cur < end) {
        পূর্ণসংখ্যা c = _PyTok_SourceByte(&tok->source, cur++);
        যদি (c == '\0') {
            ফেরত 0;
        }
        যদি (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            চালু;
        }
        যদি (c != '#') {
            ফেরত 1;
        }
        যখন (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            যদি (c == '\0' || c == '\n') {
                বিরতি;
            }
            cur++;
        }
    }
    ফেরত 0;
}

পূর্ণসংখ্যা
_PyTokenizer_IsInteractive(ধ্রুবক কাঠামো tok_state *tok)
{
    ফেরত _PyTok_ReaderIsInteractive(tok);
}

শূন্য
_PyTokenizer_StopInteractive(কাঠামো tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
