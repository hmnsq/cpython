#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "tokenizer.h"
#अन्तर्भाव "reader.h"
#अन्तर्भाव "reader_internal.h"
#अन्तर्भाव "../lexer/state.h"

पूर्णांक
_PyTokenizer_GetStatus(स्थिरांक संरचना tok_state *tok)
{
    परत tok->done;
}

पूर्णांक
_PyTokenizer_GetNewlineRequirements(स्थिरांक संरचना tok_state *tok)
{
    परत tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(स्थिरांक संरचना tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->ओळ_start < 0
            ? -1 : (पूर्णांक)(tok->cur - tok->ओळ_start)},
        .cursor = tok->cur,
        .आदान_span = {tok->buf_offset, tok->inp},
        .ओळ_span = {tok->ओळ_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != रिक्त && tok->reader->fp != मानक_निवेश,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    जर (tok->level > 0) {
        पूर्णांक level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    परत info;
}

स्थिरांक अक्षर *
_PyToken_TextView(स्थिरांक संरचना tok_state *tok, स्थिरांक संरचना token *token,
                  Py_ssize_t *length)
{
    प्रतिज्ञा(length != रिक्त);
    जर (token->span.start < 0) {
        प्रतिज्ञा(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        परत "";
    }
    परत _PyTok_SourceSpanView(&tok->source, token->span, length);
}

स्थिरांक अक्षर *
_PyTokenizer_SpanView(स्थिरांक संरचना tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    परत _PyTok_SourceSpanView(&tok->source, span, length);
}

शून्य
_PyToken_GetView(स्थिरांक संरचना tok_state *tok, स्थिरांक संरचना token *token,
                 _PyToken_View *view)
{
    प्रतिज्ञा(view != रिक्त);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_ओळ_start = tok->ओळ_start;
    view->ओळ_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->ओळ_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

स्थिरांक अक्षर *
Py_Tokenizer_ओळ_View(स्थिरांक संरचना tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    परत Py_Tok_Source_ओळ_View(&tok->source, lineno, length);
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
_PyTokenizer_SetOptions(संरचना tok_state *tok, पूर्णांक extra_tokens,
                        पूर्णांक type_comments)
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
Py_Tokenizer_Allow_Incomplete_आदान(संरचना tok_state *tok, पूर्णांक allow)
{
    tok->allow_incomplete_आदान = allow != 0;
}

पूर्णांक
_PyTokenizer_HasTrailingStatement(स्थिरांक संरचना tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    पर्यंत (cur < end) {
        पूर्णांक c = _PyTok_SourceByte(&tok->source, cur++);
        जर (c == '\0') {
            परत 0;
        }
        जर (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            सुरूठेवा;
        }
        जर (c != '#') {
            परत 1;
        }
        पर्यंत (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            जर (c == '\0' || c == '\n') {
                थांबा;
            }
            cur++;
        }
    }
    परत 0;
}

पूर्णांक
_PyTokenizer_IsInteractive(स्थिरांक संरचना tok_state *tok)
{
    परत _PyTok_ReaderIsInteractive(tok);
}

शून्य
_PyTokenizer_StopInteractive(संरचना tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
