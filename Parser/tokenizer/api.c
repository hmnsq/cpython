#അന്തര്ഭാവ "Python.h"
#അന്തര്ഭാവ "errcode.h"
#അന്തര്ഭാവ "pycore_token.h"

#അന്തര്ഭാവ "tokenizer.h"
#അന്തര്ഭാവ "reader.h"
#അന്തര്ഭാവ "reader_internal.h"
#അന്തര്ഭാവ "../lexer/state.h"

പൂർണ്ണസംഖ്യ
_PyTokenizer_GetStatus(സ്ഥിരം ഘടന tok_state *tok)
{
    തിരികെ tok->done;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_GetNewlineRequirements(സ്ഥിരം ഘടന tok_state *tok)
{
    തിരികെ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(സ്ഥിരം ഘടന tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->വരി_start < 0
            ? -1 : (പൂർണ്ണസംഖ്യ)(tok->cur - tok->വരി_start)},
        .cursor = tok->cur,
        .നിവേശം_span = {tok->buf_offset, tok->inp},
        .വരി_span = {tok->വരി_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ശൂന്യ && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    എങ്കിൽ (tok->level > 0) {
        പൂർണ്ണസംഖ്യ level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    തിരികെ info;
}

സ്ഥിരം അക്ഷരം *
_PyToken_TextView(സ്ഥിരം ഘടന tok_state *tok, സ്ഥിരം ഘടന token *token,
                  Py_ssize_t *length)
{
    പ്രതിജ്ഞാ(length != ശൂന്യ);
    എങ്കിൽ (token->span.start < 0) {
        പ്രതിജ്ഞാ(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        തിരികെ "";
    }
    തിരികെ _PyTok_SourceSpanView(&tok->source, token->span, length);
}

സ്ഥിരം അക്ഷരം *
_PyTokenizer_SpanView(സ്ഥിരം ഘടന tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    തിരികെ _PyTok_SourceSpanView(&tok->source, span, length);
}

ശൂന്യം
_PyToken_GetView(സ്ഥിരം ഘടന tok_state *tok, സ്ഥിരം ഘടന token *token,
                 _PyToken_View *view)
{
    പ്രതിജ്ഞാ(view != ശൂന്യ);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_വരി_start = tok->വരി_start;
    view->വരി_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->വരി_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

സ്ഥിരം അക്ഷരം *
Py_Tokenizer_വരി_View(സ്ഥിരം ഘടന tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    തിരികെ Py_Tok_Source_വരി_View(&tok->source, lineno, length);
}

ശൂന്യം
_PyTokenizer_SetContext(ഘടന tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

ശൂന്യം
_PyTokenizer_SetOptions(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ extra_tokens,
                        പൂർണ്ണസംഖ്യ type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

ശൂന്യം
_PyTokenizer_ImplyDedents(ഘടന tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

ശൂന്യം
Py_Tokenizer_Allow_Incomplete_നിവേശം(ഘടന tok_state *tok, പൂർണ്ണസംഖ്യ allow)
{
    tok->allow_incomplete_നിവേശം = allow != 0;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_HasTrailingStatement(സ്ഥിരം ഘടന tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    ആകുമ്പോൾ (cur < end) {
        പൂർണ്ണസംഖ്യ c = _PyTok_SourceByte(&tok->source, cur++);
        എങ്കിൽ (c == '\0') {
            തിരികെ 0;
        }
        എങ്കിൽ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            തുടരുക;
        }
        എങ്കിൽ (c != '#') {
            തിരികെ 1;
        }
        ആകുമ്പോൾ (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            എങ്കിൽ (c == '\0' || c == '\n') {
                നിർത്തുക;
            }
            cur++;
        }
    }
    തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
_PyTokenizer_IsInteractive(സ്ഥിരം ഘടന tok_state *tok)
{
    തിരികെ _PyTok_ReaderIsInteractive(tok);
}

ശൂന്യം
_PyTokenizer_StopInteractive(ഘടന tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
