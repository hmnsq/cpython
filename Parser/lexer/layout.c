#ضمن "Python.h"
#ضمن "errcode.h"
#ضمن "pycore_token.h"

#ضمن "lexer_internal.h"
#ضمن "../tokenizer/helpers.h"
#ضمن "../tokenizer/reader.h"

#وصف_ڪريو TABSIZE 8
#وصف_ڪريو ALTTABSIZE 1

صحيح
Py_Lexer_Continue_سٽ(بنية tok_state *tok)
{
    صحيح c = tok_nextc(tok);
    إذا (c == '\r') {
        c = tok_nextc(tok);
    }
    إذا (c != '\n') {
        tok->پورو = E_LINECONT;
        أرجع -1;
    }
    c = tok_nextc(tok);
    إذا (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    إذا (c == EOF) {
        tok->پورو = E_EOF;
        tok->cur = tok->inp;
        أرجع -1;
    } وإلا {
        tok_backup(tok, c);
    }
    أرجع c;
}


ساكن صحيح
update_indentation(بنية tok_state *tok, صحيح کانو, صحيح altcol)
{
    lexer_layout_state *layout = &tok->layout;
    إذا (کانو == layout->ڍير[layout->اونهائي].ڪالم) {
        إذا (altcol != layout->ڍير[layout->اونهائي].alternate_ڪالم) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    وإلا إذا (کانو > layout->ڍير[layout->اونهائي].ڪالم) {
        إذا (layout->اونهائي + 1 >= MAXINDENT) {
            tok->پورو = E_TOODEEP;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol <= layout->ڍير[layout->اونهائي].alternate_ڪالم) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
        layout->pending++;
        layout->ڍير[++layout->اونهائي] = (indentation_level){کانو, altcol};
    }
    وإلا {
        طالما (layout->اونهائي > 0 &&
            کانو < layout->ڍير[layout->اونهائي].ڪالم) {
            layout->pending--;
            layout->اونهائي--;
        }
        إذا (کانو != layout->ڍير[layout->اونهائي].ڪالم) {
            tok->پورو = E_DEDENT;
            tok->cur = tok->inp;
            أرجع -1;
        }
        إذا (altcol != layout->ڍير[layout->اونهائي].alternate_ڪالم) {
            _PyTokenizer_indenterror(tok);
            أرجع -1;
        }
    }
    أرجع 0;
}

صحيح
Py_Lexer_Begin_سٽ(بنية tok_state *tok)
{
    أكد(tok->layout.at_bol);
    صحيح c;
    صحيح blankline = 0;
    صحيح کانو = 0;
    صحيح altcol = 0;
    tok->layout.at_bol = 0;
    صحيح cont_سٽ_کانو = 0;
    لكل (;;) {
        c = tok_nextc(tok);
        إذا (c == ' ') {
            کانو++, altcol++;
        }
        وإلا إذا (c == '\t') {
            کانو = (کانو / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        وإلا إذا (c == '\014')  {/* Control-L (formfeed) */
            کانو = altcol = 0; /* For Emacs users */
        }
        وإلا إذا (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_سٽ_کانو = cont_سٽ_کانو ? cont_سٽ_کانو : کانو;
            إذا ((c = Py_Lexer_Continue_سٽ(tok)) == -1) {
                أرجع -1;
            }
        }
        وإلا إذا (c == EOF && PyErr_Occurred()) {
            أرجع -1;
        }
        وإلا {
            اكسر;
        }
    }
    tok_backup(tok, c);
    إذا (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_سٽ_کانو || کانو)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    إذا (c == '#' || c == '\n' || c == '\r') {
        صحيح interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        إذا (کانو == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        وإلا إذا (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            کانو = altcol = 0;
        }
        وإلا {
            blankline = 1; /* Ignore completely */
        }
    }
    إذا (!blankline && tok->level == 0) {
        کانو = cont_سٽ_کانو ? cont_سٽ_کانو : کانو;
        altcol = cont_سٽ_کانو ? cont_سٽ_کانو : altcol;
        إذا (update_indentation(tok, کانو, altcol) < 0) {
            أرجع -1;
        }
    }
    أرجع blankline;
}

صحيح
_PyLexer_IndentationToken(بنية tok_state *tok, بنية token *token)
{
    أكد(tok->layout.pending != 0);
    _PyTok_Off p_شروعات = -1;
    _PyTok_Off p_پڇاڙي = -1;
    إذا (tok->layout.pending < 0) {
        إذا (tok->tok_extra_ٽوڪن) {
            p_شروعات = tok->cur;
            p_پڇاڙي = tok->cur;
        }
        tok->layout.pending++;
        أرجع Py_Lexer_token_تياري(tok, token, DEDENT, p_شروعات, p_پڇاڙي);
    }
    وإلا {
        إذا (tok->tok_extra_ٽوڪن) {
            p_شروعات = tok->buf_وٿي;
            p_پڇاڙي = tok->cur;
        }
        tok->layout.pending--;
        أرجع Py_Lexer_token_تياري(tok, token, INDENT, p_شروعات, p_پڇاڙي);
    }
}

صحيح
_PyLexer_Newline(بنية tok_state *tok, بنية token *token, صحيح blankline)
{
    إذا (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    إذا (blankline || tok->level > 0) {
        إذا (!tok->tok_extra_ٽوڪن) {
            أرجع 0;
        }
    }
    وإلا إذا (!tok->layout.comment_newline || !tok->tok_extra_ٽوڪن) {
        أرجع Py_Lexer_token_تياري(tok, token, NEWLINE,
                                    tok->شروعات, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    أرجع Py_Lexer_token_تياري(tok, token, NL, tok->شروعات, tok->cur);
}

فراغ
_PyLexer_ImplyDedents(بنية tok_state *tok)
{
    إذا (tok->layout.اونهائي != 0) {
        tok->layout.pending = -tok->layout.اونهائي;
        tok->layout.اونهائي = 0;
    }
}
