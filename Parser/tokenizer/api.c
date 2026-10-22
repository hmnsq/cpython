#అన్తర్భావ "Python.h"
#అన్తర్భావ "errcode.h"
#అన్తర్భావ "pycore_token.h"

#అన్తర్భావ "tokenizer.h"
#అన్తర్భావ "reader.h"
#అన్తర్భావ "reader_internal.h"
#అన్తర్భావ "../lexer/state.h"

పూర్ణాంకం
Py_Tokenizer_Get_స్థితి(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    తిరిగి tok->పూర్తి;
}

పూర్ణాంకం
_PyTokenizer_GetNewlineRequirements(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    తిరిగి tok->needs_newline;
}

_PyTokenizer_Info
_PyTokenizer_GetInfo(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    _PyTokenizer_Info info = {
        .స్థితి_2 = tok->పూర్తి,
        .implicit_newline = tok->implicit_newline,
        .diagnostic = tok->diagnostic,
        .location = {tok->lineno, tok->పంక్తి_ప్రారంభం < 0
            ? -1 : (పూర్ణాంకం)(tok->cur - tok->పంక్తి_ప్రారంభం)},
        .cursor = tok->cur,
        .ఆదానం_పరిధి = {tok->buf_స్థానభ్రంశం, tok->inp},
        .పంక్తి_పరిధి = {tok->పంక్తి_ప్రారంభం, tok->inp},
        .level = tok->level,
        .delimiter_loc = {-1, -1},
        .లోపల_formatted_string = tok->ftstring_లోతు != 0,
        .is_interactive = _PyTok_ReaderIsInteractive(tok),
        .is_file = tok->reader->fp != ఖాళీ && tok->reader->fp != stdin,
        .filename = tok->filename,
        .module = tok->module,
        .encoding = tok->reader->encoding,
    };
    ఒకవేళ (tok->level > 0) {
        పూర్ణాంకం level = tok->level - 1;
        info.delimiter = tok->parenstack[level];
        info.delimiter_loc = (_PyTok_Loc){
            tok->parenlinenostack[level], tok->parencolstack[level]};
    }
    తిరిగి info;
}

స్థిరరాశి అక్షరం *
Py_Token_పాఠ్యం_View(స్థిరరాశి నిర్మాణం tok_state *tok, స్థిరరాశి నిర్మాణం token *token,
                  Py_ssize_t *నిడివి)
{
    ప్రతిజ్ఞా(నిడివి != ఖాళీ);
    ఒకవేళ (token->పరిధి.ప్రారంభం < 0) {
        ప్రతిజ్ఞా(token->పరిధి.ప్రారంభం == -1 && token->పరిధి.చివర == -1);
        *నిడివి = 0;
        తిరిగి "";
    }
    తిరిగి Py_Tok_ఆధారం_పరిధి_View(&tok->ఆధారం, token->పరిధి, నిడివి);
}

స్థిరరాశి అక్షరం *
Py_Tokenizer_పరిధి_View(స్థిరరాశి నిర్మాణం tok_state *tok, Py_Tok_పరిధి పరిధి,
                      Py_ssize_t *నిడివి)
{
    తిరిగి Py_Tok_ఆధారం_పరిధి_View(&tok->ఆధారం, పరిధి, నిడివి);
}

శూన్య
_PyToken_GetView(స్థిరరాశి నిర్మాణం tok_state *tok, స్థిరరాశి నిర్మాణం token *token,
                 _PyToken_View *view)
{
    ప్రతిజ్ఞా(view != ఖాళీ);
    view->పాఠ్యం = Py_Token_పాఠ్యం_View(tok, token, &view->నిడివి);
    view->చివర_పంక్తి_ప్రారంభం = tok->పంక్తి_ప్రారంభం;
    view->పంక్తి_పరిధి = (Py_Tok_పరిధి){
        ISSTRINGLIT(token->type)
            ? token->పరిధి.ప్రారంభం - token->ప్రారంభం_loc.byte_నిలువు : tok->పంక్తి_ప్రారంభం,
        tok->inp,
    };
    view->implicit_newline = tok->implicit_newline;
    view->at_eof = tok->పూర్తి == E_EOF;
}

స్థిరరాశి అక్షరం *
Py_Tokenizer_పంక్తి_View(స్థిరరాశి నిర్మాణం tok_state *tok, Py_ssize_t lineno,
                      Py_ssize_t *నిడివి)
{
    తిరిగి Py_Tok_ఆధారం_పంక్తి_View(&tok->ఆధారం, lineno, నిడివి);
}

శూన్య
Py_Tokenizer_సమితి_Context(నిర్మాణం tok_state *tok, PyObject *filename,
                        PyObject *module)
{
    Py_XINCREF(filename);
    Py_XINCREF(module);
    Py_XSETREF(tok->filename, filename);
    Py_XSETREF(tok->module, module);
}

శూన్య
Py_Tokenizer_సమితి_Options(నిర్మాణం tok_state *tok, పూర్ణాంకం extra_టోకెన్లు,
                        పూర్ణాంకం type_comments)
{
    tok->tok_extra_టోకెన్లు = extra_టోకెన్లు;
    tok->type_comments = type_comments;
}

శూన్య
_PyTokenizer_ImplyDedents(నిర్మాణం tok_state *tok)
{
    _PyLexer_ImplyDedents(tok);
}

శూన్య
Py_Tokenizer_Allow_Incomplete_ఆదానం(నిర్మాణం tok_state *tok, పూర్ణాంకం allow)
{
    tok->allow_incomplete_ఆదానం = allow != 0;
}

పూర్ణాంకం
_PyTokenizer_HasTrailingStatement(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    _PyTok_Off cur = tok->cur;
    _PyTok_Off చివర = tok->ఆధారం.పునాది_స్థానభ్రంశం + tok->ఆధారం.len;
    అయితే (cur < చివర) {
        పూర్ణాంకం c = Py_Tok_ఆధారం_Byte(&tok->ఆధారం, cur++);
        ఒకవేళ (c == '\0') {
            తిరిగి 0;
        }
        ఒకవేళ (c == ' ' || c == '\t' || c == '\n' || c == '\014') {
            కొనసాగు;
        }
        ఒకవేళ (c != '#') {
            తిరిగి 1;
        }
        అయితే (cur < చివర) {
            c = Py_Tok_ఆధారం_Byte(&tok->ఆధారం, cur);
            ఒకవేళ (c == '\0' || c == '\n') {
                ఆపు;
            }
            cur++;
        }
    }
    తిరిగి 0;
}

పూర్ణాంకం
_PyTokenizer_IsInteractive(స్థిరరాశి నిర్మాణం tok_state *tok)
{
    తిరిగి _PyTok_ReaderIsInteractive(tok);
}

శూన్య
_PyTokenizer_StopInteractive(నిర్మాణం tok_state *tok)
{
    _PyTok_ReaderStopInteractive(tok);
}
