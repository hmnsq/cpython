#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "tokenizer.h"
#include "reader.h"
#include "reader_internal.h"
#include "../lexer/state.h"

số_nguyên
_PyTokenizer_GetStatus(const struct tok_state *tok)
{
    trả_về tok->done;
}

số_nguyên
_PyTokenizer_GetNewlineRequirements(const struct tok_state *tok)
{
    trả_về tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(const struct tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->line_start < 0
            ? -1 : (số_nguyên)(tok->cur - tok->line_start)},
        .cursor = tok->cur,
        .input_span = {tok->buf_offset, tok->inp},
        .line_span = {tok->line_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != NULL && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    nếu (tok->level > 0) {
        số_nguyên level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    trả_về info;
}

const char *
_PyToken_TextView(const struct tok_state *tok, const struct token *token,
                  Py_ssize_t *length)
{
    assert(length != NULL);
    nếu (token->span.start < 0) {
        assert(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        trả_về "";
    }
    trả_về _PyTok_SourceSpanView(&tok->source, token->span, length);
}

const char *
_PyTokenizer_SpanView(const struct tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    trả_về _PyTok_SourceSpanView(&tok->source, span, length);
}

void
_PyToken_GetView(const struct tok_state *tok, const struct token *token,
                 _PyToken_View *view)
{
    assert(view != NULL);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_line_start = tok->line_start;
    view->line_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->line_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

const char *
_PyTokenizer_LineView(const struct tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    trả_về _PyTok_SourceLineView(&tok->source, lineno, length);
}

void
_PyTokenizer_SetContext(struct tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

void
_PyTokenizer_SetOptions(struct tok_state *tok, số_nguyên extra_tokens,
                        số_nguyên type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

void
_PyTokenizer_ImplyDedents(struct tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

void
_PyTokenizer_AllowIncompleteInput(struct tok_state *tok, số_nguyên allow)
{
    tok->allow_incomplete_input = allow != 0;
}

số_nguyên
_PyTokenizer_HasTrailingStatement(const struct tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    trong_khi (cur < end) {
        số_nguyên c = _PyTok_SourceByte(&tok->source, cur++);
        nếu (c == '\0') {
            trả_về 0;
        }
        nếu (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            continue;
        }
        nếu (c != '#') {
            trả_về 1;
        }
        trong_khi (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            nếu (c == '\0' || c == '\n') {
                break;
            }
            cur++;
        }
    }
    trả_về 0;
}

số_nguyên
_PyTokenizer_IsInteractive(const struct tok_state *tok)
{
    trả_về _PyTok_ReaderIsInteractive(tok);
}

void
_PyTokenizer_StopInteractive(struct tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
