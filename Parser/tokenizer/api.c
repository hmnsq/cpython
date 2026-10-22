#include "Python.h"
#include "errcode.h"
#include "pycore_token.h"

#include "tokenizer.h"
#include "reader.h"
#include "reader_internal.h"
#include "../lexer/state.h"

ចំនួនគត់
_PyTokenizer_GetStatus(const sa_nracanaa tok_state *tok)
{
    ត្រឡប់ tok->done;
}

ចំនួនគត់
_PyTokenizer_GetNewlineRequirements(const sa_nracanaa tok_state *tok)
{
    ត្រឡប់ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(const sa_nracanaa tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->line_start < 0
            ? -1 : (ចំនួនគត់)(tok->cur - tok->line_start)},
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
    ប្រសិនបើ (tok->level > 0) {
        ចំនួនគត់ level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ត្រឡប់ info;
}

const var_na *
_PyToken_TextView(const sa_nracanaa tok_state *tok, const sa_nracanaa token *token,
                  Py_ssize_t *length)
{
    assert(length != NULL);
    ប្រសិនបើ (token->span.start < 0) {
        assert(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        ត្រឡប់ "";
    }
    ត្រឡប់ _PyTok_SourceSpanView(&tok->source, token->span, length);
}

const var_na *
_PyTokenizer_SpanView(const sa_nracanaa tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ត្រឡប់ _PyTok_SourceSpanView(&tok->source, span, length);
}

void
_PyToken_GetView(const sa_nracanaa tok_state *tok, const sa_nracanaa token *token,
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

const var_na *
_PyTokenizer_LineView(const sa_nracanaa tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    ត្រឡប់ _PyTok_SourceLineView(&tok->source, lineno, length);
}

void
_PyTokenizer_SetContext(sa_nracanaa tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

void
_PyTokenizer_SetOptions(sa_nracanaa tok_state *tok, ចំនួនគត់ extra_tokens,
                        ចំនួនគត់ type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

void
_PyTokenizer_ImplyDedents(sa_nracanaa tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

void
_PyTokenizer_AllowIncompleteInput(sa_nracanaa tok_state *tok, ចំនួនគត់ allow)
{
    tok->allow_incomplete_input = allow != 0;
}

ចំនួនគត់
_PyTokenizer_HasTrailingStatement(const sa_nracanaa tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    ខណៈពេល (cur < end) {
        ចំនួនគត់ c = _PyTok_SourceByte(&tok->source, cur++);
        ប្រសិនបើ (c == '\0') {
            ត្រឡប់ 0;
        }
        ប្រសិនបើ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            continue;
        }
        ប្រសិនបើ (c != '#') {
            ត្រឡប់ 1;
        }
        ខណៈពេល (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            ប្រសិនបើ (c == '\0' || c == '\n') {
                bha_nga;
            }
            cur++;
        }
    }
    ត្រឡប់ 0;
}

ចំនួនគត់
_PyTokenizer_IsInteractive(const sa_nracanaa tok_state *tok)
{
    ត្រឡប់ _PyTok_ReaderIsInteractive(tok);
}

void
_PyTokenizer_StopInteractive(sa_nracanaa tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
