#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "tokenizer.h"
#include "reader.h"
#include "reader_internal.h"
#include "../lexer/state.h"

цяло
_PyTokenizer_GetStatus(const struct tok_state *tok)
{
    върни tok->done;
}

цяло
_PyTokenizer_GetNewlineRequirements(const struct tok_state *tok)
{
    върни tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(const struct tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->line_start < 0
            ? -1 : (цяло)(tok->cur - tok->line_start)},
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
    ако (tok->level > 0) {
        цяло level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    върни info;
}

const char *
_PyToken_TextView(const struct tok_state *tok, const struct token *token,
                  Py_ssize_t *length)
{
    assert(length != NULL);
    ако (token->span.start < 0) {
        assert(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        върни "";
    }
    върни _PyTok_SourceSpanView(&tok->source, token->span, length);
}

const char *
_PyTokenizer_SpanView(const struct tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    върни _PyTok_SourceSpanView(&tok->source, span, length);
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
    върни _PyTok_SourceLineView(&tok->source, lineno, length);
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
_PyTokenizer_SetOptions(struct tok_state *tok, цяло extra_tokens,
                        цяло type_comments)
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
_PyTokenizer_AllowIncompleteInput(struct tok_state *tok, цяло allow)
{
    tok->allow_incomplete_input = allow != 0;
}

цяло
_PyTokenizer_HasTrailingStatement(const struct tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    докато (cur < end) {
        цяло c = _PyTok_SourceByte(&tok->source, cur++);
        ако (c == '\0') {
            върни 0;
        }
        ако (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            continue;
        }
        ако (c != '#') {
            върни 1;
        }
        докато (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            ако (c == '\0' || c == '\n') {
                break;
            }
            cur++;
        }
    }
    върни 0;
}

цяло
_PyTokenizer_IsInteractive(const struct tok_state *tok)
{
    върни _PyTok_ReaderIsInteractive(tok);
}

void
_PyTokenizer_StopInteractive(struct tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
