#ಅನ್ತರ್ಭಾವ "Python.h"
#ಅನ್ತರ್ಭಾವ "errcode.h"
#ಅನ್ತರ್ಭಾವ "pycore_token.h"

#ಅನ್ತರ್ಭಾವ "tokenizer.h"
#ಅನ್ತರ್ಭಾವ "reader.h"
#ಅನ್ತರ್ಭಾವ "reader_internal.h"
#ಅನ್ತರ್ಭಾವ "../lexer/state.h"

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_GetStatus(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತ್ಯಾಗಮ tok->done;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_GetNewlineRequirements(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತ್ಯಾಗಮ tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    _PyTokenizer_Info info = {
        .status = tok->done,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->ಸಾಲು_start < 0
            ? -1 : (ಪೂರ್ಣಾಙ್ಕ)(tok->cur - tok->ಸಾಲು_start)},
        .cursor = tok->cur,
        .ಒಳಹರಿವು_span = {tok->buf_offset, tok->inp},
        .ಸಾಲು_span = {tok->ಸಾಲು_start, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .in_formatted_string = tok->ftstring_depth != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ರಿಕ್ತ && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    ಯದಿ (tok->level > 0) {
        ಪೂರ್ಣಾಙ್ಕ level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    ಪ್ರತ್ಯಾಗಮ info;
}

ಧ್ರುವ ವರ್ಣ *
_PyToken_TextView(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ಸಂರಚನಾ token *token,
                  Py_ssize_t *length)
{
    ಪ್ರತಿಜ್ಞಾ(length != ರಿಕ್ತ);
    ಯದಿ (token->span.start < 0) {
        ಪ್ರತಿಜ್ಞಾ(token->span.start == -1 && token->span.end == -1);
        *length = 0;
        ಪ್ರತ್ಯಾಗಮ "";
    }
    ಪ್ರತ್ಯಾಗಮ _PyTok_SourceSpanView(&tok->source, token->span, length);
}

ಧ್ರುವ ವರ್ಣ *
_PyTokenizer_SpanView(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok, _PyTok_Span span,
                      Py_ssize_t *length)
{
    ಪ್ರತ್ಯಾಗಮ _PyTok_SourceSpanView(&tok->source, span, length);
}

ಶೂನ್ಯ
_PyToken_GetView(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok, ಧ್ರುವ ಸಂರಚನಾ token *token,
                 _PyToken_View *view)
{
    ಪ್ರತಿಜ್ಞಾ(view != ರಿಕ್ತ);
    view->text = _PyToken_TextView(tok, token, &view->length);
    view->end_ಸಾಲು_start = tok->ಸಾಲು_start;
    view->ಸಾಲು_span = (_PyTok_Span){
        ISSTRINGLIT(token->type)
            ? token->span.start - token->start_loc.byte_col : tok->ಸಾಲು_start,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->done == E_EOF;
}

ಧ್ರುವ ವರ್ಣ *
Py_Tokenizer_ಸಾಲು_View(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *length)
{
    ಪ್ರತ್ಯಾಗಮ Py_Tok_Source_ಸಾಲು_View(&tok->source, lineno, length);
}

ಶೂನ್ಯ
_PyTokenizer_SetContext(ಸಂರಚನಾ tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

ಶೂನ್ಯ
_PyTokenizer_SetOptions(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ extra_tokens,
                        ಪೂರ್ಣಾಙ್ಕ type_comments)
{
    tok->tok_extra_tokens = extra_tokens;
    tok->type_comments = type_comments;
}

ಶೂನ್ಯ
_PyTokenizer_ImplyDedents(ಸಂರಚನಾ tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

ಶೂನ್ಯ
Py_Tokenizer_Allow_Incomplete_ಒಳಹರಿವು(ಸಂರಚನಾ tok_state *tok, ಪೂರ್ಣಾಙ್ಕ allow)
{
    tok->allow_incomplete_ಒಳಹರಿವು = allow != 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_HasTrailingStatement(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off end = tok->source.base_offset + tok->source.len;
    ಯಾವತ್ (cur < end) {
        ಪೂರ್ಣಾಙ್ಕ c = _PyTok_SourceByte(&tok->source, cur++);
        ಯದಿ (c == '\0') {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            ಅನುವರ್ತ;
        }
        ಯದಿ (c != '#') {
            ಪ್ರತ್ಯಾಗಮ 1;
        }
        ಯಾವತ್ (cur < end) {
            c = _PyTok_SourceByte(&tok->source, cur);
            ಯದಿ (c == '\0' || c == '\n') {
                ಭಙ್ಗ;
            }
            cur++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
_PyTokenizer_IsInteractive(ಧ್ರುವ ಸಂರಚನಾ tok_state *tok)
{
    ಪ್ರತ್ಯಾಗಮ _PyTok_ReaderIsInteractive(tok);
}

ಶೂನ್ಯ
_PyTokenizer_StopInteractive(ಸಂರಚನಾ tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
