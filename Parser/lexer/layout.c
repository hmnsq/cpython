#अन्तर्भाव "Python.h"
#अन्तर्भाव "errcode.h"
#अन्तर्भाव "pycore_token.h"

#अन्तर्भाव "lexer_internal.h"
#अन्तर्भाव "../tokenizer/helpers.h"
#अन्तर्भाव "../tokenizer/reader.h"

#define TABSIZE 8
#define ALTTABSIZE 1

पूर्णांक
Py_Lexer_Continue_ओळ(संरचना tok_state *tok)
{
    पूर्णांक c = tok_nextc(tok);
    जर (c == '\r') {
        c = tok_nextc(tok);
    }
    जर (c != '\n') {
        tok->done = E_LINECONT;
        परत -1;
    }
    c = tok_nextc(tok);
    जर (tok->implicit_newline == _PyTokenizer_APPEND_NEWLINE) {
        tok->needs_newline |= _PyTokenizer_NEWLINE_CONTINUATION;
    }
    जर (c == खातापूर्ण) {
        tok->done = E_EOF;
        tok->cur = tok->inp;
        परत -1;
    } अन्यथा {
        tok_backup(tok, c);
    }
    परत c;
}


स्थिर पूर्णांक
update_indentation(संरचना tok_state *tok, पूर्णांक col, पूर्णांक altcol)
{
    lexer_layout_state *layout = &tok->layout;
    जर (col == layout->चळत[layout->depth].column) {
        जर (altcol != layout->चळत[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            परत -1;
        }
    }
    अन्यथा जर (col > layout->चळत[layout->depth].column) {
        जर (layout->depth + 1 >= MAXINDENT) {
            tok->done = E_TOODEEP;
            tok->cur = tok->inp;
            परत -1;
        }
        जर (altcol <= layout->चळत[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            परत -1;
        }
        layout->pending++;
        layout->चळत[++layout->depth] = (indentation_level){col, altcol};
    }
    अन्यथा {
        पर्यंत (layout->depth > 0 &&
            col < layout->चळत[layout->depth].column) {
            layout->pending--;
            layout->depth--;
        }
        जर (col != layout->चळत[layout->depth].column) {
            tok->done = E_DEDENT;
            tok->cur = tok->inp;
            परत -1;
        }
        जर (altcol != layout->चळत[layout->depth].alternate_column) {
            _PyTokenizer_indenterror(tok);
            परत -1;
        }
    }
    परत 0;
}

पूर्णांक
Py_Lexer_Begin_ओळ(संरचना tok_state *tok)
{
    प्रतिज्ञा(tok->layout.at_bol);
    पूर्णांक c;
    पूर्णांक blankline = 0;
    पूर्णांक col = 0;
    पूर्णांक altcol = 0;
    tok->layout.at_bol = 0;
    पूर्णांक cont_ओळ_col = 0;
    साठी (;;) {
        c = tok_nextc(tok);
        जर (c == ' ') {
            col++, altcol++;
        }
        अन्यथा जर (c == '\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        }
        अन्यथा जर (c == '\014')  {/* Control-L (formfeed) */
            col = altcol = 0; /* For Emacs users */
        }
        अन्यथा जर (c == '\\') {
            // Indentation cannot be split over multiple physical lines
            // using backslashes. This means that if we found a backslash
            // preceded by whitespace, **the first one we find** determines
            // the level of indentation of whatever comes next.
            cont_ओळ_col = cont_ओळ_col ? cont_ओळ_col : col;
            जर ((c = Py_Lexer_Continue_ओळ(tok)) == -1) {
                परत -1;
            }
        }
        अन्यथा जर (c == खातापूर्ण && PyErr_Occurred()) {
            परत -1;
        }
        अन्यथा {
            थांबा;
        }
    }
    tok_backup(tok, c);
    जर (c == '\n' && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE &&
        tok->level == 0 && (cont_ओळ_col || col)) {
        // Without the newline, a whitespace-only line affects indentation.
        tok->needs_newline |= _PyTokenizer_NEWLINE_INDENT;
    }
    जर (c == '#' || c == '\n' || c == '\r') {
        पूर्णांक interactive = _PyTok_ReaderIsInteractive(tok);
        /* Lines with only whitespace and/or comments
           shouldn't affect the indentation and are
           not passed to the parser as NEWLINE tokens,
           except *totally* empty lines in interactive
           mode, which signal the end of a command group. */
        जर (col == 0 && c == '\n' && interactive) {
            blankline = 0; /* Let it through */
        }
        अन्यथा जर (interactive && tok->lineno == 1) {
            /* In interactive mode, if the first line contains
               only spaces and/or a comment, let it through. */
            blankline = 0;
            col = altcol = 0;
        }
        अन्यथा {
            blankline = 1; /* Ignore completely */
        }
    }
    जर (!blankline && tok->level == 0) {
        col = cont_ओळ_col ? cont_ओळ_col : col;
        altcol = cont_ओळ_col ? cont_ओळ_col : altcol;
        जर (update_indentation(tok, col, altcol) < 0) {
            परत -1;
        }
    }
    परत blankline;
}

पूर्णांक
_PyLexer_IndentationToken(संरचना tok_state *tok, संरचना token *token)
{
    प्रतिज्ञा(tok->layout.pending != 0);
    _PyTok_Off p_start = -1;
    _PyTok_Off p_end = -1;
    जर (tok->layout.pending < 0) {
        जर (tok->tok_extra_tokens) {
            p_start = tok->cur;
            p_end = tok->cur;
        }
        tok->layout.pending++;
        परत _PyLexer_token_setup(tok, token, DEDENT, p_start, p_end);
    }
    अन्यथा {
        जर (tok->tok_extra_tokens) {
            p_start = tok->buf_offset;
            p_end = tok->cur;
        }
        tok->layout.pending--;
        परत _PyLexer_token_setup(tok, token, INDENT, p_start, p_end);
    }
}

पूर्णांक
_PyLexer_Newline(संरचना tok_state *tok, संरचना token *token, पूर्णांक blankline)
{
    जर (!blankline && tok->implicit_newline == _PyTokenizer_IMPLY_NEWLINE) {
        // The statement's terminating newline was not in the input.
        tok->needs_newline |= _PyTokenizer_NEWLINE_STATEMENT;
    }
    tok->layout.at_bol = 1;
    जर (blankline || tok->level > 0) {
        जर (!tok->tok_extra_tokens) {
            परत 0;
        }
    }
    अन्यथा जर (!tok->layout.comment_newline || !tok->tok_extra_tokens) {
        परत _PyLexer_token_setup(tok, token, NEWLINE,
                                    tok->start, tok->cur - 1);
    }
    tok->layout.comment_newline = 0;
    परत _PyLexer_token_setup(tok, token, NL, tok->start, tok->cur);
}

शून्य
_PyLexer_ImplyDedents(संरचना tok_state *tok)
{
    जर (tok->layout.depth != 0) {
        tok->layout.pending = -tok->layout.depth;
        tok->layout.depth = 0;
    }
}
