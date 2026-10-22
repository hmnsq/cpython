#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "tokenizer.h"
#अन्तर्भाव "reader.h"
#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "../lexer/state.h"

पूर्णाङ्क
_PyTokenizer_GetStatus(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->done;
}

पूर्णाङ्क
_PyTokenizer_GetNewlineRequirements(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ध्रुव संरचना tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->पंक्ति_start < 0
            ? -1 : (पूर्णाङ्क)(tok->cur - tok->पंक्ति_start)},
        .cursor = tok->cur,
        .निवेश_span = {tok->buf_offset, tok->inp},
        .पंक्ति_span = {tok->पंक्ति_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != रिक्त && tok->reader->fp != मानक_निवेश,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    यदि (tok->level > 0) {
        पूर्णाङ्क level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    प्रत्यागम info;
}

ध्रुव वर्ण *
_PyToken_TextView(ध्रुव संरचना tok_state *tok, ध्रुव संरचना token *token,
                  Py_ssize_t *length)
{
    प्रतिज्ञा(length != रिक्त);
    यदि (token->span.start < 0) {
        प्रतिज्ञा(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        प्रत्यागम "";
    }
    प्रत्यागम _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ध्रुव वर्ण *
_PyTokenizer_SpanView(ध्रुव संरचना tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    प्रत्यागम _PyTok_SourceSpanView(&tok->source, span, length);
}

शून्य
_PyToken_GetView(ध्रुव संरचना tok_state *tok, ध्रुव संरचना token *token,
                 _PyToken_View *view)
{
    प्रतिज्ञा(view != रिक्त);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_पंक्ति_start = tok->पंक्ति_start;
    view->पंक्ति_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->पंक्ति_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ध्रुव वर्ण *
Py_Tokenizer_पंक्ति_View(ध्रुव संरचना tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    प्रत्यागम Py_Tok_Source_पंक्ति_View(&tok->source, lineno, length);
}

शून्य
_PyTokenizer_SetContext(संरचना tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

शून्य
_PyTokenizer_SetOptions(संरचना tok_state *tok, पूर्णाङ्क extra_tokens,
                        पूर्णाङ्क type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

शून्य
_PyTokenizer_ImplyDedents(संरचना tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

शून्य
Py_Tokenizer_Allow_Incomplete_निवेश(संरचना tok_state *tok, पूर्णाङ्क allow)
{
    tok->allow_incomplete_निवेश = allow != 0;
}

पूर्णाङ्क
_PyTokenizer_HasTrailingStatement(ध्रुव संरचना tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    यावत् (cur < end) {
        पूर्णाङ्क c = _PyTok_SourceByte(&tok->source, cur++);
        यदि (c == '\0') {
            प्रत्यागम 0;
        }
        यदि (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            अनुवर्त;
        }
        यदि (c != '#') {
            प्रत्यागम 1;
        }
        यावत् (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            यदि (c == '\0' || c == '\n') {
                भङ्ग;
            }
            cur++;
        }
    }
    प्रत्यागम 0;
}

पूर्णाङ्क
_PyTokenizer_IsInteractive(ध्रुव संरचना tok_state *tok)
{
    प्रत्यागम _PyTok_ReaderIsInteractive(tok);
}

शून्य
_PyTokenizer_StopInteractive(संरचना tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
