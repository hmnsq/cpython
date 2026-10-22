#ଅନ୍ତର୍ଭାଵ "Python.h"
#ଅନ୍ତର୍ଭାଵ "errcode.h"
#ଅନ୍ତର୍ଭାଵ "pycore_token.h"

#ଅନ୍ତର୍ଭାଵ "lexer_internal.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/helpers.h"
#ଅନ୍ତର୍ଭାଵ "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Lexer_Continue_ଧାଡ଼ି(ସଂରଚନା tok_state *tok)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = tok_nextc(tok);
    ଯଦି (c == '\r') {
        c = tok_nextc(tok);
    }
    ଯଦି (c != '\n') {
        tok->done = E_LINECONT;
        ଫେରନ୍ତୁ -1;
    }
    c = tok_nextc(tok);
    ଯଦି (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    ଯଦି (c == EOF) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        ଫେରନ୍ତୁ -1;
    } ଅନ୍ୟଥା {
        tok_backup(tok, c);
    }
    ଫେରନ୍ତୁ c;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
update_indentation(ସଂରଚନା tok_state *tok, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା altcol)
{
    lexer_layout_state *layout = &tok->layout;
    ଯଦି (col == layout->ଗଦା[layout->depth].column) {
        ଯଦି (altcol != layout->ଗଦା[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ଫେରନ୍ତୁ -1;
        }
    }
    ଅନ୍ୟଥା ଯଦି (col > layout->ଗଦା[layout->depth].column) {
        ଯଦି (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            ଫେରନ୍ତୁ -1;
        }
        ଯଦି (altcol <= layout->ଗଦା[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ଫେରନ୍ତୁ -1;
        }
        layout->pending++;
        layout->ଗଦା[++layout->depth] = (indentation_level){col, altcol};
    }
    ଅନ୍ୟଥା {
        ଯେତେବେଳେ (layout->depth > 0 &&
            col < layout->ଗଦା[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        ଯଦି (col != layout->ଗଦା[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            ଫେରନ୍ତୁ -1;
        }
        ଯଦି (altcol != layout->ଗଦା[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            ଫେରନ୍ତୁ -1;
        }
    }
    ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
Py_Lexer_Begin_ଧାଡ଼ି(ସଂରଚନା tok_state *tok)
{
    ପ୍ରତିଜ୍ଞା(tok->layout.at_bol);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା blankline = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା altcol = 0;
    tok->layout.at_bol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cont_ଧାଡ଼ି_col = 0;
    ପାଇଁ (;;) {
        c = tok_nextc(tok);
        ଯଦି (c == ' ') {
            col++, altcol++;
        }
        ଅନ୍ୟଥା ଯଦି (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        ଅନ୍ୟଥା ଯଦି (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        ଅନ୍ୟଥା ଯଦି (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_ଧାଡ଼ି_col = cont_ଧାଡ଼ି_col ? cont_ଧାଡ଼ି_col : col;
            ଯଦି ((c = Py_Lexer_Continue_ଧାଡ଼ି(tok)) == -1) {
                ଫେରନ୍ତୁ -1;
            }
        }
        ଅନ୍ୟଥା ଯଦି (c == EOF && PyErr_Occurred()) {
            ଫେରନ୍ତୁ -1;
        }
        ଅନ୍ୟଥା {
            ଭାଙ୍ଗ;
        }
    }
    tok_backup(tok, c);
    ଯଦି (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_ଧାଡ଼ି_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    ଯଦି (c == '#' || c == '\n' || c == '\r') {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        ଯଦି (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        ଅନ୍ୟଥା ଯଦି (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        ଅନ୍ୟଥା {
            blankline = 1; /* Ignore completely */
        }
    }
    ଯଦି (!blankline && tok->level == 0) {
        col = cont_ଧାଡ଼ି_col ? cont_ଧାଡ଼ି_col : col;
        altcol = cont_ଧାଡ଼ି_col ? cont_ଧାଡ଼ି_col : altcol;
        ଯଦି (update_indentation(tok, col, altcol) < 0) {
            ଫେରନ୍ତୁ -1;
        }
    }
    ଫେରନ୍ତୁ blankline;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_IndentationToken(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token)
{
    ପ୍ରତିଜ୍ଞା(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    ଯଦି (tok->layout.pending < 0) {
        ଯଦି (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        ଫେରନ୍ତୁ _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    ଅନ୍ୟଥା {
        ଯଦି (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        ଫେରନ୍ତୁ _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_PyLexer_Newline(ସଂରଚନା tok_state *tok, ସଂରଚନା token *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blankline)
{
    ଯଦି (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    ଯଦି (blankline || tok->level > 0) {
        ଯଦି (!tok->tok_extra_tokens) {
            ଫେରନ୍ତୁ 0;
        }
    }
    ଅନ୍ୟଥା ଯଦି (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        ଫେରନ୍ତୁ _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    ଫେରନ୍ତୁ _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

ଶୂନ୍ୟ
_PyLexer_ImplyDedents(ସଂରଚନା tok_state *tok)
{
    ଯଦି (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
