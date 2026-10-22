#అన్తర్భావ "Python.h"
#అన్తర్భావ "errcode.h"
#అన్తర్భావ "pycore_token.h"

#అన్తర్భావ "lexer_internal.h"
#అన్తర్భావ "../tokenizer/helpers.h"
#అన్తర్భావ "../tokenizer/reader.h"

#నిర్వచించు TABSIZE 8
#నిర్వచించు ALTTABSIZE 1

పూర్ణాంకం
Py_Lexer_Continue_పంక్తి(నిర్మాణం tok_state *tok)
{
    పూర్ణాంకం c = tok_nextc(tok);
    ఒకవేళ (c == '\r') {
        c = tok_nextc(tok);
    }
    ఒకవేళ (c != '\n') {
        tok->పూర్తి = E_LINECONT;
        తిరిగి -1;
    }
    c = tok_nextc(tok);
    ఒకవేళ (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ఒకవేళ (c == EOF) {
        tok->పూర్తి = E_EOF;
        tok->cur = tok->inp;
        తిరిగి -1;
    } లేకపోతే {
        tok_backup(tok, c);
    }
    తిరిగి c;
}


స్థిర పూర్ణాంకం
update_indentation(నిర్మాణం tok_state *tok, పూర్ణాంకం నిలువు, పూర్ణాంకం altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ఒకవేళ (నిలువు == layout->దొంతర[layout->లోతు].స్తంభం) {
        ఒకవేళ (altcol != layout->దొంతర[layout->లోతు].alternate_స్తంభం) {
            _PyTokenizer_indenterror(tok);
            తిరిగి -1;
        }
    }
    లేకపోతే ఒకవేళ (నిలువు > layout->దొంతర[layout->లోతు].స్తంభం) {
        ఒకవేళ (layout->లోతు + 1 >= MAXINDENT) {
            tok->పూర్తి = E_TOODEEP;
            tok->cur = tok->inp;
            తిరిగి -1;
        }
        ఒకవేళ (altcol <= layout->దొంతర[layout->లోతు].alternate_స్తంభం) {
            _PyTokenizer_indenterror(tok);
            తిరిగి -1;
        }
        layout->pending++;
        layout->దొంతర[++layout->లోతు] = (indentation_level){నిలువు, altcol};
    }
    లేకపోతే {
        అయితే (layout->లోతు > 0 &&
            నిలువు < layout->దొంతర[layout->లోతు].స్తంభం) {
            layout->pending--;
            layout->లోతు--;
        }
        ఒకవేళ (నిలువు != layout->దొంతర[layout->లోతు].స్తంభం) {
            tok->పూర్తి = E_DEDENT;
            tok->cur = tok->inp;
            తిరిగి -1;
        }
        ఒకవేళ (altcol != layout->దొంతర[layout->లోతు].alternate_స్తంభం) {
            _PyTokenizer_indenterror(tok);
            తిరిగి -1;
        }
    }
    తిరిగి 0;
}

పూర్ణాంకం
Py_Lexer_Begin_పంక్తి(నిర్మాణం tok_state *tok)
{
    ప్రతిజ్ఞా(tok->layout.at_bol);
    పూర్ణాంకం c;
    పూర్ణాంకం blankline = 0;
    పూర్ణాంకం నిలువు = 0;
    పూర్ణాంకం altcol = 0;
    tok->layout.at_bol = 0;
    పూర్ణాంకం cont_పంక్తి_నిలువు = 0;
    కొరకు (;;) {
        c = tok_nextc(tok);
        ఒకవేళ (c == ' ') {
            నిలువు++, altcol++;
        }
        లేకపోతే ఒకవేళ (c == '\t') {
            నిలువు = (నిలువు / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        లేకపోతే ఒకవేళ (c == '\014')  {/* Control-L (formfeed) */
            నిలువు = altcol = 0; /* For Emacs users */
        }
        లేకపోతే ఒకవేళ (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_పంక్తి_నిలువు = cont_పంక్తి_నిలువు ? cont_పంక్తి_నిలువు : నిలువు;
            ఒకవేళ ((c = Py_Lexer_Continue_పంక్తి(tok)) == -1) {
                తిరిగి -1;
            }
        }
        లేకపోతే ఒకవేళ (c == EOF && PyErr_Occurred()) {
            తిరిగి -1;
        }
        లేకపోతే {
            ఆపు;
        }
    }
    tok_backup(tok, c);
    ఒకవేళ (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_పంక్తి_నిలువు || నిలువు)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ఒకవేళ (c == '#' || c == '\n' || c == '\r') {
        పూర్ణాంకం interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ఒకవేళ (నిలువు == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        లేకపోతే ఒకవేళ (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            నిలువు = altcol = 0;
        }
        లేకపోతే {
            blankline = 1; /* Ignore completely */
        }
    }
    ఒకవేళ (!blankline && tok->level == 0) {
        నిలువు = cont_పంక్తి_నిలువు ? cont_పంక్తి_నిలువు : నిలువు;
        altcol = cont_పంక్తి_నిలువు ? cont_పంక్తి_నిలువు : altcol;
        ఒకవేళ (update_indentation(tok, నిలువు, altcol) < 0) {
            తిరిగి -1;
        }
    }
    తిరిగి blankline;
}

పూర్ణాంకం
_PyLexer_IndentationToken(నిర్మాణం tok_state *tok, నిర్మాణం token *token)
{
    ప్రతిజ్ఞా(tok->layout.pending != 0);
    _PyTok_Off p_ప్రారంభం = -1;
    _PyTok_Off p_చివర = -1;
    ఒకవేళ (tok->layout.pending < 0) {
        ఒకవేళ (tok->tok_extra_టోకెన్లు) {
            p_ప్రారంభం = tok->cur;
            p_చివర = tok->cur;
        }
        tok->layout.pending++;
        తిరిగి Py_Lexer_token_అమరిక(tok, token, DEDENT, p_ప్రారంభం, p_చివర);
    }
    లేకపోతే {
        ఒకవేళ (tok->tok_extra_టోకెన్లు) {
            p_ప్రారంభం = tok->buf_స్థానభ్రంశం;
            p_చివర = tok->cur;
        }
        tok->layout.pending--;
        తిరిగి Py_Lexer_token_అమరిక(tok, token, INDENT, p_ప్రారంభం, p_చివర);
    }
}

పూర్ణాంకం
_PyLexer_Newline(నిర్మాణం tok_state *tok, నిర్మాణం token *token, పూర్ణాంకం blankline)
{
    ఒకవేళ (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ఒకవేళ (blankline || tok->level > 0) {
        ఒకవేళ (!tok->tok_extra_టోకెన్లు) {
            తిరిగి 0;
        }
    }
    లేకపోతే ఒకవేళ (!tok->layout.comment_newline || !tok->tok_extra_టోకెన్లు) {
        తిరిగి Py_Lexer_token_అమరిక(tok, token, NEWLINE,
                                    tok->ప్రారంభం, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    తిరిగి Py_Lexer_token_అమరిక(tok, token, NL, tok->ప్రారంభం, tok->cur);
}

శూన్య
_PyLexer_ImplyDedents(నిర్మాణం tok_state *tok)
{
    ఒకవేళ (tok->layout.లోతు != 0) {
        tok->layout.pending = -tok->layout.లోతు;
        tok->layout.లోతు = 0;
    }
}
